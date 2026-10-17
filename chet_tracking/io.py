"""Read raw CHeT hits from files (Geant4 output, DAQ dumps) into padded arrays.

Expected flat table, one row per SiPM hit:
    event (int), channel (int, global CHeT channel id), time (ns), [truth (int)]
Supported: .npz / .csv / .root (TTree via ``uproot``, optional dependency).
If your channel numbering differs, remap to ``CHeTGeometry`` ids before calling
``pad_hits`` (see ``geometry.tables['layer_offset']``).
"""
from __future__ import annotations

import numpy as np


def load_flat(path: str, tree: str = "hits"):
    if path.endswith(".npz"):
        d = np.load(path)
        return {k: d[k] for k in d.files}
    if path.endswith(".csv"):
        arr = np.genfromtxt(path, delimiter=",", names=True)
        return {k: arr[k] for k in arr.dtype.names}
    if path.endswith(".root"):
        import uproot  # optional
        with uproot.open(path) as f:
            return {k: np.asarray(v) for k, v in f[tree].arrays(library="np").items()}
    raise ValueError(f"unsupported file type: {path}")


def pad_hits(xp, flat: dict):
    """Flat hit table -> raw dict of [E, H] device arrays (event ids re-indexed 0..E-1)."""
    ev = np.asarray(flat["event"]).astype(np.int64)
    order = np.argsort(ev, kind="stable")
    ev = ev[order]
    uniq, inv, counts = np.unique(ev, return_inverse=True, return_counts=True)
    E, H = len(uniq), int(counts.max())
    start = np.concatenate([[0], np.cumsum(counts)[:-1]])
    pos = np.arange(len(ev)) - start[inv]
    out = {}
    for key, dtype in (("channel", np.int64), ("time", np.float64), ("truth", np.int32)):
        if key not in flat:
            continue
        a = np.zeros((E, H), dtype=dtype)
        a[inv, pos] = np.asarray(flat[key])[order]
        out[key] = a
    valid = np.zeros((E, H), dtype=bool)
    valid[inv, pos] = True
    raw = dict(ch=xp.asarray(out["channel"]), t=xp.asarray(out["time"]), valid=xp.asarray(valid))
    raw["truth"] = xp.asarray(out["truth"]) if "truth" in out else xp.full((E, H), -1, dtype=xp.int32)
    return raw, uniq


def save_tracks(path: str, tracks: dict, **extra):
    np.savez_compressed(path, **tracks, **extra)
