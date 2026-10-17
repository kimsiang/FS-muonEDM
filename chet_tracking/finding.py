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
from .geometry import C_MM_PER_NS, CHeTGeometry

TWO_PI = 2 * np.pi


@dataclass
class RecoConfig:
    crossing_dt_ns: float = 2.0     # u/v coincidence window
    seed_dt_ns: float = 6.0         # max time spread of a triplet
    track_dt_ns: float = 10.0       # hit-to-seed time compatibility
    tol_xy_mm: float = 2.0          # |rho - R| for hit attachment
    tol_z_mm: float = 8.0           # z residual for hit attachment
    seed_z_tol_mm: float = 8.0      # triplet z consistency
    seed_sigma_z_mm: float = 3.0
    seed_sigma_t_ns: float = 0.6
    r_min_mm: float = 5.0
    r_max_mm: float = 95.0
    orbit_tol_mm: float = 6.0       # track circle must touch the muon orbit
    min_hits: int = 4
    max_tracks: int = 6
    max_turn_index: int = 4
    seed_window: int = 10           # triplets from points within W neighbours in time
    max_seeds: int = 3000
    work_budget: float = 4e6        # max (#events x #seeds x #points) per sub-batch
    sigma_xy_mm: float = 0.8
    outlier_nsigma: float = 4.0
    fit_iterations: int = 5


@lru_cache(maxsize=256)
def _triplets(n: int, window: int) -> np.ndarray:
    """Index triplets (i, i+a, i+b), 1 <= a < b <= window, on time-sorted points.
    O(n * window^2) instead of O(n^3)."""
    if n < 3:
        return np.zeros((0, 3), dtype=np.int64)
    off = np.array(list(combinations(range(1, window + 1), 2)), dtype=np.int64)
    i = np.repeat(np.arange(n), len(off))
    t = np.stack([i, i + np.tile(off[:, 0], n), i + np.tile(off[:, 1], n)], 1)
    return t[t[:, 2] < n]


def _hypotheses(xp):
    ref, shift = [], []
    for r in range(3):
        others = [q for q in range(3) if q != r]
        for na in (0, 1):
            for nb in (0, 1):
                sh = [0, 0, 0]
                sh[others[0]], sh[others[1]] = na, nb
                ref.append(r)
                shift.append(sh)
    return xp.asarray(np.array(ref)), xp.asarray(np.array(shift, dtype=np.float64))


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
    tri = xp.asarray(_triplets(P, cfg.seed_window))
    if tri.shape[0] == 0:
        return None
    i, j, k = tri[:, 0], tri[:, 1], tri[:, 2]
    g = {key: (pts[key][:, i], pts[key][:, j], pts[key][:, k])
         for key in ("x", "y", "z", "t", "cyl", "hu", "hv")}
    vi, vj, vk = valid[:, i], valid[:, j], valid[:, k]
    ci, cj, ck = g["cyl"]
    ti, tj, tk = g["t"]
    # >= 2 different cylinders (tracks with R < ~25 mm only see the inner or
    # the outer pair) and no raw SiPM hit used twice (a point and its ghost)
    m = vi & vj & vk & ~((ci == cj) & (cj == ck))
    for key in ("hu", "hv"):
        u1, u2, u3 = g[key]
        m &= (u1 != u2) & (u2 != u3) & (u1 != u3)
    tmax = xp.maximum(xp.maximum(ti, tj), tk)
    tmin = xp.minimum(xp.minimum(ti, tj), tk)
    m &= (tmax - tmin) < cfg.seed_dt_ns

    xc, yc, R, good = _circle3(g["x"][0], g["y"][0], g["x"][1], g["y"][1],
                               g["x"][2], g["y"][2], xp)
    dc = xp.hypot(xc, yc)
    r0 = geom.orbit_radius_mm
    m &= good & (R > cfg.r_min_mm) & (R < cfg.r_max_mm)
    m &= (xp.abs(dc - R) < r0 + cfg.orbit_tol_mm) & (dc + R > r0 - cfg.orbit_tol_mm)
    # keep only surviving triplets before the expensive hypothesis test
    src = {f"{key}{q}": g[key][q] for key in ("x", "y", "z", "t") for q in range(3)}
    src.update(xc=xc, yc=yc, R=R)
    c, m, _ = compact(xp, m, src)
    g = {key: (c[f"{key}0"], c[f"{key}1"], c[f"{key}2"]) for key in ("x", "y", "z", "t")}
    xc, yc, R = c["xc"], c["yc"], c["R"]

    # z model.  Turn ordering of the three points is ambiguous for curlers, so
    # test 12 hypotheses (reference point x next-turn offsets of the other two)
    # and keep the one most consistent in z and in time of flight.
    X3 = xp.stack(g["x"], -1)
    Y3 = xp.stack(g["y"], -1)
    Z3 = xp.stack(g["z"], -1)
    T3 = xp.stack(g["t"], -1)
    psi3 = xp.arctan2(Y3 - yc[..., None], X3 - xc[..., None])
    REF, SHIFT = _hypotheses(xp)
    base = xp.mod(psi3[..., :, None] - psi3[..., None, :], TWO_PI)        # [..., ref, q]
    dpsi = base[..., REF, :] + TWO_PI * SHIFT                              # [..., H, 3]
    Zh = Z3[..., None, :]
    mu_d, mu_z = dpsi.mean(-1), Zh.mean(-1)
    sdd = ((dpsi - mu_d[..., None]) ** 2).sum(-1)
    sdz = ((dpsi - mu_d[..., None]) * (Zh - mu_z[..., None])).sum(-1)
    okh = sdd > 1e-4
    h = sdz / xp.where(okh, sdd, 1.0)
    a_ = mu_z - h * mu_d
    zres = xp.abs(Zh - a_[..., None] - h[..., None] * dpsi).max(-1)
    # time of flight along the helix (beta ~ 1)
    dl = xp.sqrt(R[..., None] ** 2 + h**2)[..., None] * dpsi / C_MM_PER_NS
    tres = T3[..., None, :] - dl
    tres = tres - tres.mean(-1, keepdims=True)
    chi = (zres / cfg.seed_sigma_z_mm) ** 2 + (tres**2).sum(-1) / cfg.seed_sigma_t_ns**2
    chi = xp.where(okh & (zres < cfg.seed_z_tol_mm), chi, xp.inf)
    hb = xp.argmin(chi, axis=-1)[..., None]
    m &= xp.isfinite(xp.take_along_axis(chi, hb, -1)[..., 0])
    h = xp.take_along_axis(h, hb, -1)[..., 0]
    a = xp.take_along_axis(a_, hb, -1)[..., 0]
    ref = REF[hb]
    psi_ref = xp.take_along_axis(psi3, ref, -1)[..., 0]
    x_ref = xp.take_along_axis(X3, ref, -1)[..., 0]
    y_ref = xp.take_along_axis(Y3, ref, -1)[..., 0]
    t_ref = T3.min(-1)
    seeds, svalid, _ = compact(xp, m, dict(xc=xc, yc=yc, R=R, h=h, a=a, psi_ref=psi_ref,
                                           x_ref=x_ref, y_ref=y_ref, t_ref=t_ref),
                               max_len=cfg.max_seeds)
    return seeds, svalid


def _residuals(xp, px, py, pz, pt, sp, cfg: RecoConfig):
    """xy, z and time residuals of points w.r.t. helix parameters ``sp``
    (arrays broadcast against the point arrays)."""
    dx, dy = px - sp["xc"], py - sp["yc"]
    rxy = xp.abs(xp.hypot(dx, dy) - sp["R"])
    dpsi = xp.mod(sp["psi_ref"] - xp.arctan2(dy, dx), TWO_PI)
    h, a = sp["h"], sp["a"]
    pitch = TWO_PI * h
    safe = xp.abs(pitch) > 1e-3
    n = xp.where(safe, xp.rint((pz - a - h * dpsi) / xp.where(safe, pitch, 1.0)), 0.0)
    n = xp.clip(n, -1, cfg.max_turn_index)
    rz = xp.abs(pz - a - h * (dpsi + TWO_PI * n))
    dt = pt - sp["t_ref"]
    return rxy, rz, dt


def _compat(xp, pts, valid, seeds, svalid, cfg: RecoConfig):
    """[E, S, P] boolean: point p lies on the helix of seed s."""
    sp = {k: v[:, :, None] for k, v in seeds.items()}
    q = {k: pts[k][:, None, :] for k in ("x", "y", "z", "t")}
    rxy, rz, dt = _residuals(xp, q["x"], q["y"], q["z"], q["t"], sp, cfg)
    ok = (valid[:, None, :] & svalid[:, :, None] & (rxy < cfg.tol_xy_mm)
          & (rz < cfg.tol_z_mm) & (dt > -cfg.seed_dt_ns) & (dt < cfg.track_dt_ns))
    chi = (rxy / cfg.sigma_xy_mm) ** 2 + (rz / (0.5 * cfg.tol_z_mm)) ** 2
    return ok, chi


def _greedy(xp, pts, valid, seeds, compat, chi_all, n_raw, cfg: RecoConfig):
    """Pick the seed with most free compatible points, claim its raw SiPM hits,
    and drop every other space point (ghost) built from a claimed hit."""
    E, P = valid.shape
    T = cfg.max_tracks
    free = valid.copy()
    ar = xp.arange(E)
    hu, hv = pts["hu"], pts["hv"]
    assign = xp.zeros((E, T, P), dtype=bool)
    tvalid = xp.zeros((E, T), dtype=bool)
    tseed = {k: xp.zeros((E, T), dtype=v.dtype) for k, v in seeds.items()}
    for it in range(T):
        cf = compat & free[:, None, :]
        nfree = cf.sum(-1)
        # tie-break equal hit counts by the summed normalised residuals
        score = nfree - 1e-3 * xp.where(cf, chi_all, 0.0).sum(-1)
        best = xp.argmax(score, axis=1)
        ok = nfree[ar, best] >= cfg.min_hits
        if not bool(ok.any()):
            break
        sel = compat[ar, best] & free & ok[:, None]
        # within the track, two points sharing a raw hit: keep the better one
        sp = {k: v[ar, best][:, None] for k, v in seeds.items()}
        rxy, rz, _ = _residuals(xp, pts["x"], pts["y"], pts["z"], pts["t"], sp, cfg)
        chi = (rxy / cfg.sigma_xy_mm) ** 2 + (rz / (0.5 * cfg.tol_z_mm)) ** 2
        share = ((hu[:, :, None] == hu[:, None, :]) | (hv[:, :, None] == hv[:, None, :]))
        share &= sel[:, :, None] & sel[:, None, :]
        idx = xp.arange(P)
        worse = (chi[:, :, None] > chi[:, None, :]) | (
            (chi[:, :, None] == chi[:, None, :]) & (idx[None, :, None] > idx[None, None, :]))
        sel &= ~(share & worse).any(-1)
        ok &= sel.sum(1) >= cfg.min_hits
        sel &= ok[:, None]
        # claim raw hits
        used = xp.zeros((E, n_raw), dtype=bool)
        eb, pb = xp.nonzero(sel)
        used[eb, hu[eb, pb]] = True
        used[eb, hv[eb, pb]] = True
        ghost = xp.take_along_axis(used, hu, 1) | xp.take_along_axis(used, hv, 1)
        assign[:, it] = sel
        tvalid[:, it] = ok
        free &= ~(sel | ghost)
        for k in tseed:
            tseed[k][:, it] = seeds[k][ar, best]
    return assign, tvalid, tseed


def find_tracks(xp, pts, valid, geom: CHeTGeometry, cfg: RecoConfig):
    """Return candidate tracks as padded [N, Ph] hit arrays + seed parameters.

    Output dict keys: hit arrays (x, y, z, t, cyl, truth), ``hit_valid``,
    ``event`` (index within the chunk), and seed_* parameters for the fitter.
    """
    E, P = valid.shape
    # time-order the points of every event so seeds use time neighbours
    order = xp.argsort(xp.where(valid, pts["t"], xp.inf), axis=1)
    pts = {k: xp.take_along_axis(a, order, 1) for k, a in pts.items()}
    valid = xp.take_along_axis(valid, order, 1)
    counts = valid.sum(1)
    n_raw = int(xp.maximum(pts["hu"], pts["hv"]).max()) + 1
    # process events in buckets of similar size (largest first)
    perm = xp.argsort(-counts)
    counts_h = [int(c) for c in counts[perm].tolist()]
    out_parts = []
    e0 = 0
    while e0 < E:
        pmax_all = max(counts_h[e0], 3)
        n_tri = max(len(_triplets(pmax_all, cfg.seed_window)), 1)
        n_seed = min(cfg.max_seeds, n_tri)
        sub = max(1, int(cfg.work_budget // max(n_seed * pmax_all, 4 * n_tri)))
        e1 = min(E, e0 + sub)
        idx = perm[e0:e1]
        pmax = max(counts_h[e0], 1)
        v = valid[idx, :pmax]
        p = {k: a[idx, :pmax] for k, a in pts.items()}
        sd = _seeds(xp, p, v, geom, cfg)
        if sd is None:
            e0 = e1
            continue
        seeds, svalid = sd
        compat, chi_all = _compat(xp, p, v, seeds, svalid, cfg)
        assign, tvalid, tseed = _greedy(xp, p, v, seeds, compat, chi_all, n_raw, cfg)
        Es, T = tvalid.shape
        rows = xp.nonzero(tvalid.reshape(-1))[0]
        if rows.size == 0:
            e0 = e1
            continue
        amask = assign.reshape(Es * T, -1)[rows]
        ev = idx[rows // T]
        hit_src = {k: xp.broadcast_to(a[:, None, :], (Es, T, a.shape[1])).reshape(Es * T, -1)[rows]
                   for k, a in p.items() if k in ("x", "y", "z", "t", "cyl", "truth", "ghost")}
        hits, hvalid, _ = compact(xp, amask, hit_src)
        part = dict(hits)
        part["hit_valid"] = hvalid
        part["event"] = ev
        for k, a in tseed.items():
            part["seed_" + k] = a.reshape(-1)[rows]
        out_parts.append(part)
        e0 = e1
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
