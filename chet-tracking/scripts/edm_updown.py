"""Demo: up/down positron asymmetry vs decay time with an injected spin tilt.

The frozen-spin EDM signal is a vertical spin component growing with time,
seen as an up/down (pz > 0 vs pz < 0) asymmetry of the decay positrons.  This
script injects an (exaggerated) tilt in the toy MC, reconstructs with CHeT
tracking, and compares reconstructed vs true asymmetry.  Decay time is taken
from MC truth here (in data it comes from the entrance trigger + track time).

    python scripts/edm_updown.py --events 6000 --device auto --tilt 0.15
"""
import argparse

import numpy as np

from chet_tracking import RecoConfig, SimConfig, run


def asym(up, n):
    a = (2 * up - n) / np.maximum(n, 1)
    return a, np.sqrt(np.maximum(1 - a**2, 0) / np.maximum(n, 1))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--events", type=int, default=6000)
    ap.add_argument("--device", default="auto")
    ap.add_argument("--tilt", type=float, default=0.15, help="spin tilt rate [rad/us]")
    ap.add_argument("--plot", default="edm_updown.png")
    a = ap.parse_args()

    sim = SimConfig(spin_tilt_rad_per_us=a.tilt)
    trk, tru, info = run(a.events, a.device, sim=sim, reco=RecoConfig(dtype="float32"))
    M = tru["valid"].shape[1]
    good = (trk["truth"] >= 0) & (trk["purity"] >= 0.6)
    e, s = trk["event"][good], trk["truth"][good]
    t_rec, up_rec = tru["t_decay_us"][e, s], trk["pz"][good] > 0
    acc = tru["valid"] & (tru["n_cross"] >= 3)
    t_tru, up_tru = tru["t_decay_us"][acc], tru["pz"][acc] > 0

    edges = np.array([0, 1, 2, 3, 4.5, 6.5, 10.0])
    mid = 0.5 * (edges[1:] + edges[:-1])
    print(f"{info['device']}, {info['reco_events_per_s']:.0f} evt/s; tilt {a.tilt} rad/us")
    print(f"{'t [us]':>8s} {'A_true':>9s} {'A_reco':>9s} {'+-':>7s} {'N_reco':>7s}")
    rows = []
    for lo, hi, m in zip(edges[:-1], edges[1:], mid):
        st, sr = (t_tru >= lo) & (t_tru < hi), (t_rec >= lo) & (t_rec < hi)
        at, _ = asym(up_tru[st].sum(), st.sum())
        ar, er = asym(up_rec[sr].sum(), sr.sum())
        rows.append((m, at, ar, er))
        print(f"{m:8.2f} {at:9.3f} {ar:9.3f} {er:7.3f} {sr.sum():7d}")

    try:
        import matplotlib
        matplotlib.use("Agg")
        import matplotlib.pyplot as plt
    except ImportError:
        return
    r = np.array(rows)
    fig, ax = plt.subplots(figsize=(6.4, 4.2))
    ax.plot(r[:, 0], r[:, 1], "o", ms=7, mfc="none", mec="#00A9F4", mew=1.5, label="truth (accepted)")
    ax.errorbar(r[:, 0], r[:, 2], r[:, 3], fmt="o", ms=5, color="#2251FF", lw=1, label="CHeT reconstructed")
    ax.axhline(0, color="#051C2C", lw=0.5, alpha=0.5)
    for sp in ("top", "right"):
        ax.spines[sp].set_visible(False)
    ax.set_xlabel("decay time [µs]")
    ax.set_ylabel("up/down asymmetry")
    ax.set_title(f"Injected spin tilt {a.tilt} rad/µs (toy MC)", loc="left", fontsize=11)
    ax.legend(frameon=False, fontsize=9)
    fig.text(0.01, 0.01, f"Source: chet-tracking toy MC, {a.events} events; tilt exaggerated for illustration",
             fontsize=7, color="#555")
    fig.tight_layout(rect=(0, 0.03, 1, 1))
    fig.savefig(a.plot, dpi=150)
    print(f"plot -> {a.plot}")


if __name__ == "__main__":
    main()
