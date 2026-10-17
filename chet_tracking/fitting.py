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
from .geometry import K_PT, CHeTGeometry

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


def _z_fit(xp, z, dpsi, wz, a, h, max_turn, n_iter=3):
    for _ in range(n_iter):
        pitch = TWO_PI * h[:, None]
        safe = xp.abs(pitch) > 1e-3
        n = xp.where(safe, xp.rint((z - a[:, None] - h[:, None] * dpsi) / xp.where(safe, pitch, 1.0)), 0.0)
        n = xp.clip(n, -1, max_turn)
        U = dpsi + TWO_PI * n
        sw = wz.sum(1)
        su, sz = (wz * U).sum(1), (wz * z).sum(1)
        suu, suz = (wz * U * U).sum(1), (wz * U * z).sum(1)
        det = sw * suu - su**2
        ok = det > 1e-9 * xp.maximum(sw * suu, 1e-30)
        det = xp.where(ok, det, 1.0)
        h = xp.where(ok, (sw * suz - su * sz) / det, h)
        a = xp.where(ok, (suu * sz - su * suz) / det, a)
    return a, h, U


def fit_tracks(xp, cand: dict, geom: CHeTGeometry, gd: dict, cfg: RecoConfig):
    x, y, z = cand["x"], cand["y"], cand["z"]
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
        a, h, U = _z_fit(xp, z, dpsi, w / sz**2, a, h, cfg.max_turn_index)
        r_xy = (xp.hypot(x - xc[:, None], y - yc[:, None]) - R[:, None]) / cfg.sigma_xy_mm
        r_z = (z - a[:, None] - h[:, None] * U) / sz
        keep = (xp.abs(r_xy) < cfg.outlier_nsigma) & (xp.abs(r_z) < cfg.outlier_nsigma)
        w_new = w * keep
        enough = w_new.sum(1) >= 3
        w = xp.where(enough[:, None], w_new, w)

    nh = w.sum(1)
    chi2 = (w * (r_xy**2 + r_z**2)).sum(1)
    ndf = xp.maximum(2 * nh - 5, 1)

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
                vx=xc + R * xp.cos(psi_v), vy=yc + R * xp.sin(psi_v), vz=z_v,
                xc=xc, yc=yc, R=R, h=h, nhits=nh.astype(xp.int32),
                chi2_ndf=chi2 / ndf, truth=truth, purity=purity, ghost_frac=ghost_frac)


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
