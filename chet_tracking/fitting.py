"""Batched helix fit of all track candidates at once (CPU or GPU).

1. geometric circle fit in xy (Gauss-Newton on (xc, yc, R), batched 3x3 solves)
2. linear z-vs-turning-angle fit with iterative turn-number assignment
3. outlier rejection and refit
4. kinematics at the decay vertex = intersection of track circle with the
   muon orbit (the one just before the first hit along the trajectory)
"""
from __future__ import annotations

import numpy as np

from .finding import RecoConfig
from .geometry import C_MM_PER_NS, K_PT, CHeTGeometry

TWO_PI = 2 * np.pi


def _circle_gn(xp, x, y, w, xc, yc, R, n_iter, sigma):
    wt = w / sigma**2
    for _ in range(n_iter):
        dx, dy = x - xc[:, None], y - yc[:, None]
        rho = xp.maximum(xp.hypot(dx, dy), 1e-9)
        res = rho - R[:, None]
        J = xp.stack([-dx / rho, -dy / rho, -xp.ones_like(rho)], axis=-1)    # [N,P,3]
        JTJ = xp.einsum("np,npi,npj->nij", wt, J, J)
        JTr = xp.einsum("np,npi,np->ni", wt, J, res)
        lam = 1e-9 * xp.trace(JTJ, axis1=1, axis2=2)[:, None, None] + 1e-12
        delta = xp.linalg.solve(JTJ + lam * xp.eye(3, dtype=x.dtype)[None], -JTr[..., None])[..., 0]
        xc, yc, R = xc + delta[:, 0], yc + delta[:, 1], R + delta[:, 2]
    return xc, yc, xp.abs(R)


def _zt_fit(xp, z, t, dpsi, w, sz, st, R, a, h, max_turn, n_iter=4):
    """Joint z / time-of-flight fit along the unwrapped turning angle U.

    z_i = a + h U_i ,  t_i = t0 + L U_i / c ,  L = sqrt(R^2 + h^2) (beta ~ 1)
    The turn number of every hit (U_i = dpsi_i + 2 pi n_i) is chosen to
    minimise the combined z and time residual, which resolves the up/down
    (h -> -h) mirror ambiguity of curling positrons.
    """
    nn = xp.arange(-1, max_turn + 1, dtype=z.dtype)
    Un = dpsi[..., None] + TWO_PI * nn                          # [N,P,K]
    wz, wt = w / sz**2, w / st**2
    L = xp.sqrt(R**2 + h**2) / C_MM_PER_NS
    t0 = (wt * (t - L[:, None] * dpsi)).sum(1) / xp.maximum(wt.sum(1), 1e-30)
    for _ in range(n_iter):
        cz = (z[..., None] - a[:, None, None] - h[:, None, None] * Un) / sz[..., None]
        ct = (t[..., None] - t0[:, None, None] - L[:, None, None] * Un) / st
        best = xp.argmin(cz**2 + ct**2, axis=-1)[..., None]
        U = xp.take_along_axis(Un, best, -1)[..., 0]
        sw = wz.sum(1)
        su, szz = (wz * U).sum(1), (wz * z).sum(1)
        suu, suz = (wz * U * U).sum(1), (wz * U * z).sum(1)
        det = sw * suu - su**2
        ok = det > 1e-9 * xp.maximum(sw * suu, 1e-30)
        det = xp.where(ok, det, 1.0)
        h = xp.where(ok, (sw * suz - su * szz) / det, h)
        a = xp.where(ok, (suu * szz - su * suz) / det, a)
        L = xp.sqrt(R**2 + h**2) / C_MM_PER_NS
        t0 = (wt * (t - L[:, None] * U)).sum(1) / xp.maximum(wt.sum(1), 1e-30)
    rz = (z - a[:, None] - h[:, None] * U) / sz
    rt = (t - t0[:, None] - L[:, None] * U) / st
    chi2 = (w * (rz**2 + rt**2)).sum(1)
    return a, h, t0, U, rz, rt, chi2


def fit_tracks(xp, cand: dict, geom: CHeTGeometry, gd: dict, cfg: RecoConfig):
    x, y, z, t = cand["x"], cand["y"], cand["z"], cand["t"]
    fdt = x.dtype
    w = cand["hit_valid"].astype(fdt)
    sz = gd["cyl_sigma_z"][cand["cyl"]].astype(fdt)
    xc, yc, R = cand["seed_xc"], cand["seed_yc"], cand["seed_R"]
    a, h = cand["seed_a"], cand["seed_h"]
    x_ref, y_ref = cand["seed_x_ref"], cand["seed_y_ref"]

    for _ in range(3):   # fit -> reject outliers -> refit
        xc, yc, R = _circle_gn(xp, x, y, w, xc, yc, R, cfg.fit_iterations, cfg.sigma_xy_mm)
        psi = xp.arctan2(y - yc[:, None], x - xc[:, None])
        psi_ref = xp.arctan2(y_ref - yc, x_ref - xc)
        dpsi = xp.mod(psi_ref[:, None] - psi, TWO_PI)
        # both up/down hypotheses, keep the better joint z + time fit
        best = None
        for sign in (1.0, -1.0):
            res = _zt_fit(xp, z, t, dpsi, w, sz, cfg.sigma_t_ns, R, a, sign * h, cfg.max_turn_index)
            if best is None:
                best = res
            else:
                better = res[-1] < best[-1]
                best = tuple(xp.where(better[:, None] if r.ndim == 2 else better, r, b)
                             for r, b in zip(res, best))
        a, h, t0, U, r_z, r_t, _ = best
        r_xy = (xp.hypot(x - xc[:, None], y - yc[:, None]) - R[:, None]) / cfg.sigma_xy_mm
        keep = ((xp.abs(r_xy) < cfg.outlier_nsigma) & (xp.abs(r_z) < cfg.outlier_nsigma)
                & (xp.abs(r_t) < cfg.outlier_nsigma))
        w_new = w * keep
        enough = w_new.sum(1) >= 3
        w = xp.where(enough[:, None], w_new, w)

    R = xp.maximum(R, 1e-6)     # empty candidate slots
    nh = w.sum(1)
    chi2 = (w * (r_xy**2 + r_z**2 + r_t**2)).sum(1)
    ndf = xp.maximum(3 * nh - 6, 1)

    # --- vertex on the muon orbit and kinematics ------------------------------
    U_masked = xp.where(w > 0, U, xp.inf)
    first = xp.argmin(U_masked, axis=1)[:, None]
    psi_first = xp.take_along_axis(psi, first, 1)[:, 0]
    U_first = xp.take_along_axis(U, first, 1)[:, 0]
    d = xp.maximum(xp.hypot(xc, yc), 1e-9)
    beta = xp.arctan2(yc, xc)
    r0 = geom.orbit_radius_mm
    acs = xp.arccos(xp.clip((r0**2 - d**2 - R**2) / (2 * R * d), -1, 1))
    cand_psi = xp.stack([beta + acs, beta - acs], -1)
    delta = xp.mod(cand_psi - psi_first[:, None], TWO_PI)
    pick = xp.argmin(delta, axis=1)[:, None]
    psi_v = xp.take_along_axis(cand_psi, pick, 1)[:, 0]
    delta_v = xp.take_along_axis(delta, pick, 1)[:, 0]
    z_v = a + h * U_first - h * delta_v

    pt = K_PT * geom.bfield_T * R
    pz = pt * h / R
    p = xp.hypot(pt, pz)
    theta = xp.arctan2(pt, pz)
    phi = xp.arctan2(-xp.cos(psi_v), xp.sin(psi_v))   # clockwise tangent (e+)

    truth, purity = _majority(xp, cand["truth"], w > 0)
    ghost_frac = ((cand["ghost"] & (w > 0)).sum(1) / xp.maximum(nh, 1)) if "ghost" in cand else xp.zeros_like(nh)
    return dict(event=cand["event"], p=p, pt=pt, pz=pz, theta=theta, phi=phi,
                t0=t0, vx=xc + R * xp.cos(psi_v), vy=yc + R * xp.sin(psi_v), vz=z_v,
                xc=xc, yc=yc, R=R, h=h, nhits=nh.astype(xp.int32),
                hit_w=w > 0, chi2_ndf=chi2 / ndf, truth=truth, purity=purity, ghost_frac=ghost_frac)


def _majority(xp, tr, m):
    eq = (tr[:, :, None] == tr[:, None, :]) & m[:, :, None] & m[:, None, :] & (tr[:, :, None] >= 0)
    cnt = eq.sum(-1)
    best = xp.argmax(cnt, axis=1)[:, None]
    lab = xp.take_along_axis(tr, best, 1)[:, 0]
    c = xp.take_along_axis(cnt, best, 1)[:, 0]
    n = xp.maximum(m.sum(1), 1)
    lab = xp.where(c > 0, lab, -1)
    return lab, c / n


def remove_clones(xp, trk: dict, dr_mm: float = 6.0, dc_mm: float = 8.0, dh_mm: float = 4.0):
    """Mask of tracks to keep: within one event, tracks with nearly identical
    helices (centre, radius, pitch) are clones; keep the one with most hits."""
    N = trk["R"].shape[0]
    if N == 0:
        return xp.zeros(0, dtype=bool)
    ev = trk["event"]
    same = ev[:, None] == ev[None, :]
    close = (same & (xp.abs(trk["R"][:, None] - trk["R"][None, :]) < dr_mm)
             & (xp.hypot(trk["xc"][:, None] - trk["xc"][None, :],
                         trk["yc"][:, None] - trk["yc"][None, :]) < dc_mm)
             & (xp.abs(trk["h"][:, None] - trk["h"][None, :]) < dh_mm))
    nh = trk["nhits"].astype(xp.float64) - 1e-3 * trk["chi2_ndf"]
    i = xp.arange(N)
    better = (nh[None, :] > nh[:, None]) | ((nh[None, :] == nh[:, None]) & (i[None, :] < i[:, None]))
    return ~(close & better).any(1)
