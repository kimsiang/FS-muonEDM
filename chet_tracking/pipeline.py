"""End-to-end reconstruction chain and performance evaluation."""
from __future__ import annotations

import numpy as np

from .backend import StageTimer, device_name, get_xp, is_gpu, to_host
from .finding import RecoConfig, find_tracks
from .fitting import fit_tracks, remove_clones
from .geometry import CHeTGeometry
from .hits import build_crossings
from .simulate import SimConfig, simulate_events


def reconstruct(xp, raw: dict, geom: CHeTGeometry, reco: RecoConfig | None = None,
                timer: StageTimer | None = None, gd: dict | None = None):
    """Raw SiPM hits [E, H] (ch, t, valid, optional truth) -> fitted tracks (device)."""
    reco = reco or RecoConfig()
    gd = gd or geom.device_tables(xp)
    timer = timer or StageTimer(xp)
    if "truth" not in raw:
        raw = dict(raw, truth=xp.full(raw["ch"].shape, -1, dtype=xp.int32))
    with timer("crossings"):
        pts, pvalid = build_crossings(xp, raw, geom, gd, reco.crossing_dt_ns)
        if reco.dtype != "float64":
            pts = {k: (v.astype(reco.dtype) if v.dtype.kind == "f" else v) for k, v in pts.items()}
    with timer("finding"):
        cand = find_tracks(xp, pts, pvalid, geom, reco)
    if cand is None:
        return None
    with timer("fitting"):
        trk = fit_tracks(xp, cand, geom, gd, reco)
        keep = remove_clones(xp, trk)
        trk = {k: v[keep] for k, v in trk.items()}
    return trk


def run(n_events: int = 2000, device: str = "cpu", chunk: int | None = None, seed: int = 1,
        geom: CHeTGeometry | None = None, sim: SimConfig | None = None,
        reco: RecoConfig | None = None, warmup: bool = True):
    """Simulate + reconstruct ``n_events`` in chunks. Returns (tracks, truth, info) on host."""
    xp = get_xp(device)
    geom = geom or CHeTGeometry()
    sim = sim or SimConfig()
    reco = reco or RecoConfig()
    gd = geom.device_tables(xp)
    chunk = chunk or (2048 if is_gpu(xp) else 256)

    if warmup:   # JIT/kernel compilation & memory pools, not timed
        raw, _ = simulate_events(min(chunk, 64), geom, xp, seed + 10_000, sim)
        reconstruct(xp, raw, geom, reco, StageTimer(xp), gd)

    timer = StageTimer(xp)
    trk_parts, tru_parts = [], []
    n_raw = n_pts = 0
    for i, e0 in enumerate(range(0, n_events, chunk)):
        ne = min(chunk, n_events - e0)
        with timer("simulation"):
            raw, truth = simulate_events(ne, geom, xp, seed + i, sim)
        n_raw += int(raw["valid"].sum())
        trk = reconstruct(xp, raw, geom, reco, timer, gd)
        with timer("transfer"):
            tru = {k: to_host(v) for k, v in truth.items()}
            tru["event"] = np.broadcast_to(np.arange(ne)[:, None] + e0, tru["valid"].shape).copy()
            tru_parts.append(tru)
            if trk is not None:
                t = {k: to_host(v) for k, v in trk.items()}
                t["event"] = t["event"] + e0
                trk_parts.append(t)
    tracks = {k: np.concatenate([t[k] for t in trk_parts]) for k in trk_parts[0]} if trk_parts else {}
    truth = {k: np.concatenate([t[k] for t in tru_parts]) for k in tru_parts[0]}
    times = timer.as_dict()
    info = dict(device=device_name(xp), n_events=n_events, chunk=chunk,
                raw_hits_per_event=n_raw / n_events, times_s=times,
                reco_time_s=sum(v for k, v in times.items() if k in ("crossings", "finding", "fitting")))
    info["reco_events_per_s"] = n_events / max(info["reco_time_s"], 1e-12)
    return tracks, truth, info


def _robust_sigma(v):
    v = v[np.isfinite(v)]
    if v.size == 0:
        return float("nan")
    return float(1.4826 * np.median(np.abs(v - np.median(v))))


def evaluate(tracks: dict, truth: dict, min_crossings: int = 3, min_purity: float = 0.6):
    """Efficiency (w.r.t. tracks crossing >= ``min_crossings`` cylinders x turns),
    fake rate and resolutions, comparable to NIM A 1080 (2025) 170760, Table 1."""
    E, M = truth["valid"].shape
    acc = truth["valid"] & (truth["n_cross"] >= min_crossings)
    out = dict(n_truth=int(truth["valid"].sum()), n_accepted=int(acc.sum()),
               n_tracks=int(len(tracks.get("p", []))))
    if not tracks:
        out.update(efficiency=0.0, fake_rate=float("nan"))
        return out
    good = (tracks["truth"] >= 0) & (tracks["purity"] >= min_purity)
    key = tracks["event"] * M + np.where(good, tracks["truth"], 0)
    # best (most hits) reco track per truth particle
    order = np.lexsort((-tracks["nhits"], key))
    order = order[good[order]]
    uniq, first = np.unique(key[order], return_index=True)
    best = order[first]
    ev, sl = uniq // M, uniq % M
    matched_acc = acc[ev, sl]
    out["efficiency"] = float(matched_acc.sum() / max(acc.sum(), 1))
    out["fake_rate"] = float((~good).mean())
    out["duplicate_rate"] = float((good.sum() - len(uniq)) / max(good.sum(), 1))
    b = best[matched_acc]
    e2, s2 = ev[matched_acc], sl[matched_acc]
    dp = tracks["p"][b] / truth["p"][e2, s2] - 1
    dth = tracks["theta"][b] - truth["theta"][e2, s2]
    dph = np.angle(np.exp(1j * (tracks["phi"][b] - truth["phi"][e2, s2])))
    out.update(sigma_p_over_p=_robust_sigma(dp), sigma_theta_mrad=1e3 * _robust_sigma(dth),
               sigma_phi_mrad=1e3 * _robust_sigma(dph), n_matched=int(len(b)))
    return out
