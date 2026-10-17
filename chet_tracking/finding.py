"""Batched track finding: triplet helix seeds + greedy hit assignment.

All events of a chunk are processed at once as dense [event, seed, point]
tensors, which is what makes the GPU fast.  Positrons are curling helices
(R up to ~75 mm, several turns), so the z-model allows turn ambiguity:
    z = z_ref + h * (dpsi + 2 pi n),   dpsi = clockwise turning from the ref point
"""
from __future__ import annotations

from dataclasses import dataclass
from functools import lru_cache
from itertools import combinations

import numpy as np

from .backend import compact
from .geometry import CHeTGeometry

TWO_PI = 2 * np.pi


@dataclass
class RecoConfig:
    crossing_dt_ns: float = 2.0     # u/v coincidence window
    seed_dt_ns: float = 6.0         # max time spread of a triplet
    track_dt_ns: float = 10.0       # hit-to-seed time compatibility
    tol_xy_mm: float = 2.0          # |rho - R| for hit attachment
    tol_z_mm: float = 8.0           # z residual for hit attachment
    seed_z_tol_mm: float = 8.0      # triplet z consistency
    r_min_mm: float = 5.0
    r_max_mm: float = 95.0
    orbit_tol_mm: float = 6.0       # track circle must touch the muon orbit
    min_hits: int = 4
    max_tracks: int = 6
    max_turn_index: int = 4
    max_seeds: int = 6000
    triplet_budget: float = 3e6     # (#events x #triplets) per sub-batch
    sigma_xy_mm: float = 0.8
    outlier_nsigma: float = 4.0
    fit_iterations: int = 5


@lru_cache(maxsize=64)
def _triplets(n: int) -> np.ndarray:
    if n < 3:
        return np.zeros((0, 3), dtype=np.int64)
    return np.array(list(combinations(range(n), 3)), dtype=np.int64)


def _circle3(x1, y1, x2, y2, x3, y3, xp):
    d = 2 * (x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2))
    good = xp.abs(d) > 1e-9
    d = xp.where(good, d, 1.0)
    s1, s2, s3 = x1**2 + y1**2, x2**2 + y2**2, x3**2 + y3**2
    xc = (s1 * (y2 - y3) + s2 * (y3 - y1) + s3 * (y1 - y2)) / d
    yc = (s1 * (x3 - x2) + s2 * (x1 - x3) + s3 * (x2 - x1)) / d
    R = xp.hypot(x1 - xc, y1 - yc)
    return xc, yc, R, good


def _seeds(xp, pts, valid, geom: CHeTGeometry, cfg: RecoConfig):
    E, P = valid.shape
    tri = xp.asarray(_triplets(P))
    if tri.shape[0] == 0:
        return None
    i, j, k = tri[:, 0], tri[:, 1], tri[:, 2]
    g = {key: (pts[key][:, i], pts[key][:, j], pts[key][:, k]) for key in ("x", "y", "z", "t", "cyl")}
    vi, vj, vk = valid[:, i], valid[:, j], valid[:, k]
    ci, cj, ck = g["cyl"]
    ti, tj, tk = g["t"]
    m = vi & vj & vk & (ci != cj) & (cj != ck) & (ci != ck)
    tmax = xp.maximum(xp.maximum(ti, tj), tk)
    tmin = xp.minimum(xp.minimum(ti, tj), tk)
    m &= (tmax - tmin) < cfg.seed_dt_ns

    xc, yc, R, good = _circle3(g["x"][0], g["y"][0], g["x"][1], g["y"][1],
                               g["x"][2], g["y"][2], xp)
    dc = xp.hypot(xc, yc)
    r0 = geom.orbit_radius_mm
    m &= good & (R > cfg.r_min_mm) & (R < cfg.r_max_mm)
    m &= (xp.abs(dc - R) < r0 + cfg.orbit_tol_mm) & (dc + R > r0 - cfg.orbit_tol_mm)

    # z model from the three points, reference = earliest hit
    X3 = xp.stack(g["x"], -1)
    Y3 = xp.stack(g["y"], -1)
    Z3 = xp.stack(g["z"], -1)
    T3 = xp.stack(g["t"], -1)
    ref = xp.argmin(T3, axis=-1)[..., None]
    psi3 = xp.arctan2(Y3 - yc[..., None], X3 - xc[..., None])
    psi_ref = xp.take_along_axis(psi3, ref, -1)[..., 0]
    dpsi = xp.mod(psi_ref[..., None] - psi3, TWO_PI)
    mu_d, mu_z = dpsi.mean(-1), Z3.mean(-1)
    sdd = ((dpsi - mu_d[..., None]) ** 2).sum(-1)
    sdz = ((dpsi - mu_d[..., None]) * (Z3 - mu_z[..., None])).sum(-1)
    m &= sdd > 1e-4
    h = sdz / xp.where(sdd > 1e-4, sdd, 1.0)
    a = mu_z - h * mu_d
    m &= xp.abs(Z3 - a[..., None] - h[..., None] * dpsi).max(-1) < cfg.seed_z_tol_mm

    x_ref = xp.take_along_axis(X3, ref, -1)[..., 0]
    y_ref = xp.take_along_axis(Y3, ref, -1)[..., 0]
    t_ref = xp.take_along_axis(T3, ref, -1)[..., 0]
    seeds, svalid, _ = compact(xp, m, dict(xc=xc, yc=yc, R=R, h=h, a=a, psi_ref=psi_ref,
                                           x_ref=x_ref, y_ref=y_ref, t_ref=t_ref),
                               max_len=cfg.max_seeds)
    return seeds, svalid


def _compat(xp, pts, valid, seeds, svalid, cfg: RecoConfig):
    """[E, S, P] boolean: point p lies on the helix of seed s."""
    S = lambda key: seeds[key][:, :, None]          # noqa: E731
    dx = pts["x"][:, None, :] - S("xc")
    dy = pts["y"][:, None, :] - S("yc")
    rxy = xp.abs(xp.hypot(dx, dy) - S("R"))
    dpsi = xp.mod(S("psi_ref") - xp.arctan2(dy, dx), TWO_PI)
    h, a = S("h"), S("a")
    pitch = TWO_PI * h
    safe = xp.abs(pitch) > 1e-3
    n = xp.where(safe, xp.rint((pts["z"][:, None, :] - a - h * dpsi) / xp.where(safe, pitch, 1.0)), 0.0)
    n = xp.clip(n, -1, cfg.max_turn_index)
    rz = xp.abs(pts["z"][:, None, :] - a - h * (dpsi + TWO_PI * n))
    dt = xp.abs(pts["t"][:, None, :] - S("t_ref"))
    return (valid[:, None, :] & svalid[:, :, None] & (rxy < cfg.tol_xy_mm)
            & (rz < cfg.tol_z_mm) & (dt < cfg.track_dt_ns))


def _greedy(xp, pts, valid, seeds, compat, cfg: RecoConfig):
    E, P = valid.shape
    T = cfg.max_tracks
    free = valid.copy()
    ar = xp.arange(E)
    assign = xp.zeros((E, T, P), dtype=bool)
    tvalid = xp.zeros((E, T), dtype=bool)
    tseed = {k: xp.zeros((E, T), dtype=v.dtype) for k, v in seeds.items()}
    for it in range(T):
        score = (compat & free[:, None, :]).sum(-1)
        best = xp.argmax(score, axis=1)
        ok = score[ar, best] >= cfg.min_hits
        if not bool(ok.any()):
            break
        sel = compat[ar, best] & free & ok[:, None]
        assign[:, it] = sel
        tvalid[:, it] = ok
        free &= ~sel
        for k in tseed:
            tseed[k][:, it] = seeds[k][ar, best]
    return assign, tvalid, tseed


def find_tracks(xp, pts, valid, geom: CHeTGeometry, cfg: RecoConfig):
    """Return candidate tracks as padded [N, Ph] hit arrays + seed parameters.

    Output dict keys: hit arrays (x, y, z, t, cyl, truth), ``hit_valid``,
    ``event`` (index within the chunk), and seed_* parameters for the fitter.
    """
    E, P = valid.shape
    n_tri = max(len(_triplets(P)), 1)
    sub = max(1, int(cfg.triplet_budget // n_tri))
    out_parts = []
    for e0 in range(0, E, sub):
        e1 = min(E, e0 + sub)
        v = valid[e0:e1]
        pmax = max(int(v.sum(1).max()), 1)
        v = v[:, :pmax]
        p = {k: a[e0:e1, :pmax] for k, a in pts.items()}
        sd = _seeds(xp, p, v, geom, cfg)
        if sd is None:
            continue
        seeds, svalid = sd
        compat = _compat(xp, p, v, seeds, svalid, cfg)
        assign, tvalid, tseed = _greedy(xp, p, v, seeds, compat, cfg)
        Es, T = tvalid.shape
        rows = xp.nonzero(tvalid.reshape(-1))[0]
        if rows.size == 0:
            continue
        amask = assign.reshape(Es * T, -1)[rows]
        ev = (rows // T) + e0
        hit_src = {k: xp.broadcast_to(a[:, None, :], (Es, T, a.shape[1])).reshape(Es * T, -1)[rows]
                   for k, a in p.items() if k in ("x", "y", "z", "t", "cyl", "truth")}
        hits, hvalid, _ = compact(xp, amask, hit_src)
        part = dict(hits)
        part["hit_valid"] = hvalid
        part["event"] = ev
        for k, a in tseed.items():
            part["seed_" + k] = a.reshape(-1)[rows]
        out_parts.append(part)
    return _concat_parts(xp, out_parts)


def _concat_parts(xp, parts):
    if not parts:
        return None
    width = max(pp["hit_valid"].shape[1] for pp in parts)
    out = {}
    for key in parts[0]:
        arrs = []
        for pp in parts:
            a = pp[key]
            if a.ndim == 2 and a.shape[1] < width:
                pad = xp.zeros((a.shape[0], width - a.shape[1]), dtype=a.dtype)
                a = xp.concatenate([a, pad], axis=1)
            arrs.append(a)
        out[key] = xp.concatenate(arrs, axis=0)
    return out
