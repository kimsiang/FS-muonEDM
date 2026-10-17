"""Stereo u/v channel coincidences -> 3D space points on each CHeT cylinder."""
from __future__ import annotations

import numpy as np

from .backend import compact
from .geometry import CHeTGeometry

TWO_PI = 2 * np.pi


def build_crossings(xp, raw: dict, geom: CHeTGeometry, gd: dict, dt_ns: float = 2.0):
    """All u x v channel pairs on the same cylinder within ``dt_ns``.

    Inverting the stereo relation phi = phi_u + z t/r = phi_v - z t/r gives
        z = (phi_v - phi_u + 2 pi k) r / (2 t)
    for every 2*pi branch k with |z| inside the detector.  Ghost combinations
    are kept here and removed later by the track finder.

    Returns points dict (x, y, z, t, cyl, truth, ch_u, ch_v) of [E, P] arrays
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

    c = xp.broadcast_to(cyl[a], pair.shape)
    r = gd["cyl_radius"][c]
    tn = gd["cyl_tan"][c]
    K = geom.max_stereo_wraps()
    k = xp.arange(-K, K + 1, dtype=xp.float64)
    dphi = (phi_ch[b] - phi_ch[a])[..., None] + TWO_PI * k         # [E,H,H,K]
    z = dphi * (r / (2 * tn))[..., None]
    sel = pair[..., None] & (xp.abs(z) <= geom.half_length_mm)

    phi = phi_ch[a][..., None] + z * (tn / r)[..., None]
    rr = r[..., None]
    x, y = rr * xp.cos(phi), rr * xp.sin(phi)
    t_prop = (geom.readout_z_mm - z) / (gd["cyl_cos"][c][..., None] * geom.v_fiber)
    tc = (0.5 * (t[a] + t[b]))[..., None] - t_prop
    truth = xp.where(tr[a] == tr[b], tr[a], -1)[..., None]
    ch_u = xp.broadcast_to(ch[a], pair.shape)[..., None]
    ch_v = xp.broadcast_to(ch[b], pair.shape)[..., None]

    pts, valid, _ = compact(xp, sel, dict(x=x, y=y, z=z, t=tc, cyl=c[..., None],
                                          truth=truth, ch_u=ch_u, ch_v=ch_v))
    return pts, valid
