"""Command line interface.

  chet-track run    --device cpu|gpu|auto --events 5000 [--float32] [--save out.npz]
  chet-track bench  --events 4000          # CPU vs GPU (if present), per stage
  chet-track reco   hits.npz --device gpu  # reconstruct real/Geant4 hits from file
  chet-track geometry                      # print the CHeT channel map summary
"""
from __future__ import annotations

import argparse
import json
import sys

import numpy as np

from .backend import get_xp, to_host
from .finding import RecoConfig
from .geometry import CHeTGeometry
from .io import load_flat, pad_hits, save_tracks
from .pipeline import evaluate, reconstruct, run
from .simulate import SimConfig


def _geom(args):
    return CHeTGeometry.from_json(args.geometry) if args.geometry else CHeTGeometry()


def _reco(args):
    return RecoConfig(dtype="float32" if args.float32 else "float64")


def _print_run(info, metrics):
    print(f"device            : {info['device']}")
    print(f"events            : {info['n_events']}  (chunk {info['chunk']})")
    print(f"raw hits / event  : {info['raw_hits_per_event']:.1f}")
    for k, v in info["times_s"].items():
        print(f"  {k:<15s} {v:8.3f} s")
    print(f"reconstruction    : {info['reco_events_per_s']:.0f} events/s")
    if metrics:
        print("performance (accepted = >= 3 crossings):")
        for k, v in metrics.items():
            print(f"  {k:<18s} {v:.4g}" if isinstance(v, float) else f"  {k:<18s} {v}")


def cmd_run(args):
    sim = SimConfig(mean_multiplicity=args.multiplicity, dark_rate_hz=args.dark_rate)
    tracks, truth, info = run(args.events, args.device, args.chunk, args.seed,
                              _geom(args), sim, _reco(args))
    metrics = evaluate(tracks, truth)
    _print_run(info, metrics)
    if args.save:
        save_tracks(args.save, tracks, **{"truth_" + k: v for k, v in truth.items()})
        print(f"saved {args.save}")
    if args.json:
        print(json.dumps(dict(info=info, metrics=metrics), indent=2, default=float))


def cmd_bench(args):
    rows = []
    for dev in ("cpu", "gpu"):
        try:
            get_xp(dev)
        except RuntimeError as exc:
            print(f"[{dev}] skipped: {exc}")
            continue
        for dt in ("float64", "float32"):
            _, _, info = run(args.events, dev, None, args.seed, _geom(args), SimConfig(),
                             RecoConfig(dtype=dt))
            rows.append((info["device"], dt, info))
    print(f"\n{'device':<28s}{'dtype':<9s}{'crossings':>10s}{'finding':>10s}{'fitting':>10s}{'evt/s':>10s}")
    for name, dt, info in rows:
        t = info["times_s"]
        print(f"{name:<28s}{dt:<9s}{t['crossings']:>10.3f}{t['finding']:>10.3f}"
              f"{t['fitting']:>10.3f}{info['reco_events_per_s']:>10.0f}")


def cmd_reco(args):
    xp = get_xp(args.device)
    geom = _geom(args)
    raw, event_ids = pad_hits(xp, load_flat(args.input, args.tree))
    trk = reconstruct(xp, raw, geom, _reco(args))
    if trk is None:
        print("no tracks found")
        return
    trk = {k: to_host(v) for k, v in trk.items()}
    trk["event_id"] = event_ids[trk["event"]]
    out = args.output or args.input.rsplit(".", 1)[0] + "_tracks.npz"
    save_tracks(out, trk)
    print(f"{len(trk['p'])} tracks from {len(event_ids)} events -> {out}")


def main(argv=None):
    ap = argparse.ArgumentParser(prog="chet-track", description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--geometry", help="JSON file with CHeTGeometry fields")
    sub = ap.add_subparsers(dest="cmd", required=True)

    r = sub.add_parser("run", help="toy MC + reconstruction + performance")
    r.add_argument("--device", default="cpu", choices=["cpu", "gpu", "cuda", "auto"])
    r.add_argument("--events", type=int, default=2000)
    r.add_argument("--chunk", type=int, default=None)
    r.add_argument("--seed", type=int, default=1)
    r.add_argument("--multiplicity", type=float, default=1.0)
    r.add_argument("--dark-rate", type=float, default=55e3, help="SiPM dark rate [Hz]")
    r.add_argument("--float32", action="store_true")
    r.add_argument("--save")
    r.add_argument("--json", action="store_true")
    r.set_defaults(func=cmd_run)

    b = sub.add_parser("bench", help="CPU vs GPU timing")
    b.add_argument("--events", type=int, default=2000)
    b.add_argument("--seed", type=int, default=1)
    b.set_defaults(func=cmd_bench)

    c = sub.add_parser("reco", help="reconstruct hits from .npz/.csv/.root")
    c.add_argument("input")
    c.add_argument("--output")
    c.add_argument("--tree", default="hits")
    c.add_argument("--device", default="auto", choices=["cpu", "gpu", "cuda", "auto"])
    c.add_argument("--float32", action="store_true")
    c.set_defaults(func=cmd_reco)

    g = sub.add_parser("geometry", help="print the channel map summary")
    g.set_defaults(func=lambda a: print(_geom(a).summary()))

    args = ap.parse_args(argv)
    args.func(args)
    return 0


if __name__ == "__main__":
    sys.exit(main())
