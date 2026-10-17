"""Fast, fully vectorised toy Monte Carlo of muEDM Phase-I decays seen by CHeT.

Runs on CPU or GPU (same ``xp`` code).  It is meant for developing and
benchmarking the tracking, not for physics: no energy loss, multiple scattering
or tile planes.  Replace it with Geant4 output via ``chet_tracking.io``.

Physics included
  * mu+ on a 30 mm orbit (frozen spin, longitudinal polarisation), Lorentz boost
  * Michel spectrum with spin-correlated asymmetry, optional EDM-like spin tilt
  * analytic helix in a uniform Bz, multi-turn crossings of every cylinder
  * stereo-fibre digitisation (4 fibres/SiPM), efficiency, time jitter,
    light propagation to the downstream SiPMs, SiPM dark-count noise, pile-up
"""
from __future__ import annotations

from dataclasses import dataclass

import numpy as np

from .backend import compact
from .geometry import C_MM_PER_NS, K_PT, M_E, M_MU, TAU_MU_NS, CHeTGeometry, digitize
from .hits import apply_dead_time

TWO_PI = 2 * np.pi


@dataclass
class SimConfig:
    mean_multiplicity: float = 1.0   # decays per readout window (Poisson, >= 1)
    max_multiplicity: int = 4
    window_ns: float = 50.0
    muon_p_mev: float = 28.0
    polarization: float = -0.95      # w.r.t. muon momentum (surface mu+ ~ -1)
    spin_tilt_rad_per_us: float = 0.0  # inject an EDM-like vertical spin tilt
    orbit_r_sigma_mm: float = 0.5
    vertex_z_sigma_mm: float = 5.0
    max_turns: int = 3
    fiber_efficiency: float = 0.95
    time_jitter_ns: float = 0.5
    dark_rate_hz: float = 55e3
    channel_dead_time_ns: float = 10.0   # same-SiPM hits closer than this merge


def _michel_x(xp, u):
    """Invert F(x) = 2x^3 - x^4 (unpolarised Michel spectrum) by bisection."""
    lo = xp.zeros_like(u)
    hi = xp.ones_like(u)
    for _ in range(40):
        mid = 0.5 * (lo + hi)
        f = 2 * mid**3 - mid**4
        below = f < u
        lo = xp.where(below, mid, lo)
        hi = xp.where(below, hi, mid)
    return 0.5 * (lo + hi)


def _sample_cos(xp, A, u):
    """Sample c in [-1, 1] from pdf ~ 1 + A c."""
    small = xp.abs(A) < 1e-6
    A_safe = xp.where(small, 1.0, A)
    disc = xp.clip(1.0 - A_safe * (2.0 - A_safe - 4.0 * u), 0.0, None)
    c = (-1.0 + xp.sqrt(disc)) / A_safe
    return xp.clip(xp.where(small, 2.0 * u - 1.0, c), -1.0, 1.0)


def simulate_events(n_events: int, geom: CHeTGeometry, xp, seed: int = 1,
                    cfg: SimConfig | None = None):
    """Return (raw_hits, truth) dictionaries of device arrays.

    raw_hits: ch, t, truth (track slot or -1 for noise), hid (crossing id), valid   -- shape [E, H]
    truth:    valid, p, pt, pz, theta, phi, vx, vy, vz, t0, t_decay_us, n_cross -- [E, M]
    """
    cfg = cfg or SimConfig()
    gd = geom.device_tables(xp)
    rng = xp.random.RandomState(seed)
    E, M, C, NT = n_events, cfg.max_multiplicity, geom.n_cyl, cfg.max_turns
    f = xp.float64

    mult = xp.clip(rng.poisson(cfg.mean_multiplicity, E), 1, M)
    trk_valid = xp.arange(M)[None, :] < mult[:, None]

    # --- muon on the orbit (clockwise for mu+ in +Bz) -------------------------
    phi_mu = TWO_PI * rng.random_sample((E, M))
    r_mu = geom.orbit_radius_mm + cfg.orbit_r_sigma_mm * rng.standard_normal((E, M))
    vx, vy = r_mu * xp.cos(phi_mu), r_mu * xp.sin(phi_mu)
    vz = cfg.vertex_z_sigma_mm * rng.standard_normal((E, M))
    nx, ny = xp.sin(phi_mu), -xp.cos(phi_mu)          # muon direction
    e_mu = np.hypot(cfg.muon_p_mev, M_MU)
    gam, bet = e_mu / M_MU, cfg.muon_p_mev / e_mu
    t_dec_us = -gam * TAU_MU_NS * 1e-3 * xp.log1p(-rng.random_sample((E, M)))

    # --- spin (frozen spin => longitudinal; optional vertical tilt) ----------
    eta = cfg.spin_tilt_rad_per_us * t_dec_us
    sgn = 1.0 if cfg.polarization >= 0 else -1.0
    sx, sy, sz = sgn * nx * xp.cos(eta), sgn * ny * xp.cos(eta), xp.sin(eta)
    pol = abs(cfg.polarization)

    # --- Michel decay in the muon rest frame ---------------------------------
    w_max = (M_MU**2 + M_E**2) / (2 * M_MU)
    x = _michel_x(xp, rng.random_sample((E, M)))
    e_rest = xp.maximum(x * w_max, M_E * 1.0001)
    p_rest = xp.sqrt(e_rest**2 - M_E**2)
    asym = pol * (2 * x - 1) / (3 - 2 * x)
    c = _sample_cos(xp, asym, rng.random_sample((E, M)))
    om = TWO_PI * rng.random_sample((E, M))
    # orthonormal basis (e1, e2) perpendicular to s (s is never parallel to x)
    ax_, ay_, az_ = xp.zeros_like(sx), xp.zeros_like(sx), xp.ones_like(sx)
    use_x = xp.abs(sz) > 0.9
    ax_, az_ = xp.where(use_x, 1.0, ax_), xp.where(use_x, 0.0, az_)
    e1x, e1y, e1z = ay_ * sz - az_ * sy, az_ * sx - ax_ * sz, ax_ * sy - ay_ * sx
    nrm = xp.sqrt(e1x**2 + e1y**2 + e1z**2)
    e1x, e1y, e1z = e1x / nrm, e1y / nrm, e1z / nrm
    e2x, e2y, e2z = sy * e1z - sz * e1y, sz * e1x - sx * e1z, sx * e1y - sy * e1x
    st = xp.sqrt(1 - c**2)
    dx = c * sx + st * (xp.cos(om) * e1x + xp.sin(om) * e2x)
    dy = c * sy + st * (xp.cos(om) * e1y + xp.sin(om) * e2y)
    dz = c * sz + st * (xp.cos(om) * e1z + xp.sin(om) * e2z)
    px, py, pz = p_rest * dx, p_rest * dy, p_rest * dz

    # --- boost to the lab along the muon direction ---------------------------
    ppar = px * nx + py * ny
    coef = (gam - 1) * ppar + gam * bet * e_rest
    px, py = px + coef * nx, py + coef * ny
    e_lab = gam * (e_rest + bet * ppar)
    pt = xp.maximum(xp.hypot(px, py), 1e-6)
    p = xp.sqrt(pt**2 + pz**2)
    beta_e = p / e_lab

    # --- helix ------------------------------------------------------------------
    R = pt / (K_PT * geom.bfield_T)
    phi_p = xp.arctan2(py, px)
    cx, cy = vx + R * xp.sin(phi_p), vy - R * xp.cos(phi_p)
    h = R * pz / pt                               # dz per radian of turning
    psi0 = xp.arctan2(vy - cy, vx - cx)
    dlen = R * p / pt                             # path length per radian
    t0 = cfg.window_ns * rng.random_sample((E, M))

    d = xp.maximum(xp.hypot(cx, cy), 1e-9)
    beta_c = xp.arctan2(cy, cx)
    r_c = gd["cyl_radius"][None, None, :]
    cosarg = (r_c**2 - d[..., None] ** 2 - R[..., None] ** 2) / (2 * R[..., None] * d[..., None])
    ok = xp.abs(cosarg) <= 1.0                                    # [E,M,C]
    acs = xp.arccos(xp.clip(cosarg, -1.0, 1.0))
    psi = beta_c[..., None, None] + xp.stack([acs, -acs], axis=-1)  # [E,M,C,2]
    dturn = xp.mod(psi0[..., None, None] - psi, TWO_PI)
    turns = TWO_PI * xp.arange(NT, dtype=f)
    dn = dturn[..., None] + turns                                  # [E,M,C,2,NT]
    zc = vz[..., None, None, None] + h[..., None, None, None] * dn
    inside = (ok[..., None, None] & (xp.abs(zc) <= geom.half_length_mm)
              & trk_valid[..., None, None, None])
    xh = cx[..., None, None] + R[..., None, None] * xp.cos(psi)
    yh = cy[..., None, None] + R[..., None, None] * xp.sin(psi)
    phih = xp.broadcast_to(xp.arctan2(yh, xh)[..., None], dn.shape)
    th = t0[..., None, None, None] + dn * (dlen / (beta_e * C_MM_PER_NS))[..., None, None, None]
    n_cross = inside.reshape(E, M, -1).sum(axis=-1)

    # --- digitisation of both stereo layers ----------------------------------
    cyl_idx = xp.arange(C)[None, None, :, None, None, None]
    s_idx = xp.arange(2)[None, None, None, None, None, :]
    layer = xp.broadcast_to(2 * cyl_idx + s_idx, dn.shape + (2,))
    phi6, z6, t6 = phih[..., None], zc[..., None], th[..., None]
    ch = digitize(xp, gd, geom.fibers_per_channel, layer, phi6, z6)
    t_prop = (geom.readout_z_mm - z6) / (gd["layer_cos"][layer] * geom.v_fiber)
    t_hit = t6 + t_prop + cfg.time_jitter_ns * rng.standard_normal(layer.shape)
    fired = inside[..., None] & (rng.random_sample(layer.shape) < cfg.fiber_efficiency)
    slot = xp.broadcast_to(xp.arange(M)[None, :, None, None, None, None], layer.shape)

    sig_ch, sig_t = ch.reshape(E, -1), t_hit.reshape(E, -1)
    sig_tr, sig_ok = slot.reshape(E, -1).astype(xp.int32), fired.reshape(E, -1)
    # crossing id (same for the u and v hit of one physical crossing)
    sig_hid = xp.broadcast_to(xp.arange(sig_ch.shape[1] // 2, dtype=xp.int32)[:, None],
                              (sig_ch.shape[1] // 2, 2)).reshape(1, -1)
    sig_hid = xp.broadcast_to(sig_hid, sig_ch.shape)

    # --- SiPM dark counts ------------------------------------------------------
    lam = cfg.dark_rate_hz * (cfg.window_ns + 10.0) * 1e-9 * geom.n_channels
    n_noise = rng.poisson(lam, E)
    K = max(int(n_noise.max()), 1)
    noi_ok = xp.arange(K)[None, :] < n_noise[:, None]
    noi_ch = rng.randint(0, geom.n_channels, (E, K)).astype(xp.int64)
    noi_t = (cfg.window_ns + 10.0) * rng.random_sample((E, K))

    all_ch = xp.concatenate([sig_ch, noi_ch], axis=1)
    all_t = xp.concatenate([sig_t, noi_t], axis=1)
    all_tr = xp.concatenate([sig_tr, xp.full((E, K), -1, dtype=xp.int32)], axis=1)
    all_ok = xp.concatenate([sig_ok, noi_ok], axis=1)
    all_hid = xp.concatenate([sig_hid, xp.full((E, K), -1, dtype=xp.int32)], axis=1)
    raw, valid, _ = compact(xp, all_ok, dict(ch=all_ch, t=all_t, truth=all_tr, hid=all_hid))
    raw["valid"] = valid
    raw = apply_dead_time(xp, raw, cfg.channel_dead_time_ns)

    theta = xp.arctan2(pt, pz)
    truth = dict(valid=trk_valid, p=p, pt=pt, pz=pz, theta=theta, phi=phi_p,
                 vx=vx, vy=vy, vz=vz, t0=t0, t_decay_us=t_dec_us, n_cross=n_cross)
    return raw, truth
