"""Stereo u/v channel coincidences -> 3D space points on each CHeT cylinder."""
from __future__ import annotations

import numpy as np

from .backend import compact
from .geometry import CHeTGeometry

TWO_PI = 2 * np.pi


def apply_dead_time(xp, raw: dict, dead_ns: float):
    """Merge repeated hits of the same SiPM channel closer than ``dead_ns``
    (keeps the earliest), as the FERS/ToT readout would.  Works on [E, H]."""
    if dead_ns <= 0:
        return raw
    ch, t, ok = raw["ch"], raw["t"], raw["valid"]
    t_span = 1e5
    key = xp.where(ok, ch.astype(xp.float64) * t_span + xp.clip(t, -1e3, t_span - 1e3 - 1) + 1e3, xp.inf)
    order = xp.argsort(key, axis=1)
    s = {k: xp.take_along_axis(v, order, 1) for k, v in raw.items()}
    same = (s["ch"][:, 1:] == s["ch"][:, :-1]) & ((s["t"][:, 1:] - s["t"][:, :-1]) < dead_ns)
    keep = s["valid"].copy()
    keep[:, 1:] &= ~(same & s["valid"][:, :-1])
    out, valid, _ = compact(xp, keep, {k: v for k, v in s.items() if k != "valid"})
    out["valid"] = valid
    return out


def build_crossings(xp, raw: dict, geom: CHeTGeometry, gd: dict, dt_ns: float = 2.0):
    """All u x v channel pairs on the same cylinder within ``dt_ns``.

    Inverting the stereo relation phi = phi_u + z t/r = phi_v - z t/r gives
        z = (phi_v - phi_u + 2 pi k) r / (2 t)
    for every 2*pi branch k with |z| inside the detector.  Ghost combinations
    are kept here and removed later by the track finder.

    Returns points dict (x, y, z, t, cyl, truth, ghost, hu, hv) of [E, P] arrays
    (hu/hv = indices of the u/v raw hits)
    and a valid mask.
    """
    ch, t, tr, ok = raw["ch"], raw["t"], raw["truth"], raw["valid"]
    lay = gd["ch_layer"][ch]
    cyl = lay // 2
    is_u = (lay % 2) == 0
    phi_ch = gd["ch_phi"][ch]

    a = (slice(None), slice(None), None)   # [E,H,1]  -> u side
    b = (slice(None), None, slice(None))   # [E,1,H]  -> v side
    pair = (ok[a] & ok[b] & is_u[a] & ~is_u[b] & (cyl[a] == cyl[b])
            & (xp.abs(t[a] - t[b]) < dt_ns))                      # [E,H,H]

    # compact the coincident pairs first: far fewer than H*H
    H = ch.shape[1]
    hu_all = xp.broadcast_to(xp.arange(H)[None, :, None], pair.shape)
    hv_all = xp.broadcast_to(xp.arange(H)[None, None, :], pair.shape)
    pr, pvalid, _ = compact(xp, pair, dict(hu=hu_all, hv=hv_all))
    hu, hv = pr["hu"], pr["hv"]
    take = lambda arr, i: xp.take_along_axis(arr, i, 1)   # noqa: E731
    c = take(cyl, hu)
    r = gd["cyl_radius"][c]
    tn = gd["cyl_tan"][c]
    phi_u, phi_v = take(phi_ch, hu), take(phi_ch, hv)
    t_u, t_v = take(t, hu), take(t, hv)
    tr_u, tr_v = take(tr, hu), take(tr, hv)

    K = geom.max_stereo_wraps()
    k = xp.arange(-K, K + 1, dtype=xp.float64)
    z = ((phi_v - phi_u)[..., None] + TWO_PI * k) * (r / (2 * tn))[..., None]   # [E,Q,K]
    sel = pvalid[..., None] & (xp.abs(z) <= geom.half_length_mm)

    phi = phi_u[..., None] + z * (tn / r)[..., None]
    rr = r[..., None]
    x, y = rr * xp.cos(phi), rr * xp.sin(phi)
    t_prop = (geom.readout_z_mm - z) / (gd["cyl_cos"][c][..., None] * geom.v_fiber)
    tc = (0.5 * (t_u + t_v))[..., None] - t_prop
    # truth: track slot if u and v come from the same positron, else -1.
    # ghost: u and v from different crossings (MC diagnostics only)
    truth = xp.where(tr_u == tr_v, tr_u, -1).astype(xp.int32)[..., None]
    hid = raw.get("hid")
    ghost = (xp.zeros(hu.shape, dtype=bool) if hid is None
             else take(hid, hu) != take(hid, hv))[..., None]
    pts, valid, _ = compact(xp, sel, dict(x=x, y=y, z=z, t=tc, cyl=c[..., None],
                                          truth=truth, ghost=ghost,
                                          hu=hu[..., None], hv=hv[..., None]))
    return pts, valid
