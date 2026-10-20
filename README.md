# chet-tracking — CPU/GPU positron tracking for the muEDM CHeT

Track reconstruction for the **Cylindrical Helix Tracker (CHeT)** of the PSI
muEDM Phase-I experiment. Every algorithm is written once against the NumPy
API and runs unchanged on

* **CPU** — `numpy`
* **GPU** — `cupy` (CUDA), selected with `--device gpu` or `get_xp("gpu")`

Events are processed in large batches as dense `[event, seed, hit]` tensors,
so there are no per-event Python loops in the hot path; this is what lets the
GPU pay off.

## Install

```bash
pip install -e .                 # CPU
pip install -e ".[gpu]"          # + CuPy for CUDA 12  (or cupy-cuda11x)
pip install -e ".[root,test]"    # + uproot for .root input, pytest
```

## Quick start

```bash
chet-track geometry                              # channel map summary
chet-track run --events 5000 --device cpu        # toy MC + reco + performance
chet-track run --events 50000 --device gpu --float32
chet-track bench --events 4000                   # CPU vs GPU, float64 vs float32
chet-track reco my_hits.npz --device auto        # your Geant4 / DAQ hits
python scripts/edm_updown.py --tilt 0.15         # up/down asymmetry demo
pytest -q tests
```

```python
from chet_tracking import CHeTGeometry, RecoConfig, get_xp, reconstruct
xp = get_xp("gpu")                    # or "cpu"
geom = CHeTGeometry(stereo_deg=(8, 8, 10, 10))
tracks = reconstruct(xp, raw_hits, geom, RecoConfig(dtype="float32"))
```

Input hits (`chet_tracking/io.py`): one row per SiPM hit with `event`,
`channel` (global CHeT channel id, see `geom.tables["layer_offset"]`), `time` [ns],
optional `truth`; `.npz`, `.csv` or a ROOT TTree.

## Reconstruction chain

| Stage | Module | What happens |
|---|---|---|
| Dead time | `hits.apply_dead_time` | same SiPM within 10 ns → one hit (earliest) |
| Space points | `hits.build_crossings` | every u×v pair on a cylinder within 2 ns → 3D point via z = (φᵥ − φᵤ + 2πk)·r / 2tanα; all valid k kept |
| Seeding | `finding._seeds` | time-ordered windowed triplets (O(N·W²)); circle through 3 points; must touch the 30 mm muon orbit; 12 turn-order hypotheses scored on z and time of flight |
| Assignment | `finding._greedy` | per step: top-K seeds per event are fitted in one batch, best (inliers − 2·χ²/ndf) wins; its raw SiPM hits are claimed and every ghost point built from them is removed |
| Fit | `fitting.fit_tracks` | Gauss–Newton circle (batched 3×3 solves), joint z + time-of-flight fit with per-hit turn numbers, both pz-sign hypotheses, 4σ outlier rejection |
| Kinematics | `fitting.fit_tracks` | vertex = track ∩ muon orbit; p, pT, pz, θ, φ, t0 |
| Clones | `fitting.remove_clones` | near-identical helices in one event → keep the best |

### Why it is built this way (CHeT specifics)

* **Curlers.** At 3 T a Michel positron has R ≤ ~75 mm and a cyclotron period
  of ~1 ns, so it crosses each cylinder several times within a few ns. Every
  u fibre of turn n combines with every v fibre of turn m, giving ghost
  points that line up into a self-consistent "mirror" helix. Making raw SiPM
  hits exclusive is what removes them.
* **Up/down ambiguity.** From z alone the turn number of a hit is ambiguous
  and h → −h (pz sign flip) often fits. Adding time of flight (σt ≈ 0.4 ns
  per crossing vs ~1 ns per turn) and fitted-quality selection cut sign flips
  from 18% to 6% and removed an up/down efficiency difference — directly
  relevant to the EDM up/down asymmetry.
* **Two-cylinder tracks.** Tracks with R ≲ 25 mm only see the inner or the
  outer pair, so seeds need ≥ 2 (not 3) distinct cylinders.
* **Stereo wrap.** With ±10° on the r = 20 mm cylinder the u/v crossing is
  two-fold ambiguous for |z| > 156 mm. No ambiguity requires
  tan α < π r / (2 L), i.e. α < 8.9° at 20 mm (L = 400 mm).

## Performance (toy MC, 5000 events, CPU, float32)

| Quantity | This code | CHeT MC, NIM A 1080 (2025) 170760 |
|---|---|---|
| Efficiency (≥ 3 crossings) | 87.8% | — |
| Fake rate | 1.5% | — |
| σp/p | 1.7% | 5% |
| σθ | 12.9 mrad | 10–30 mrad |
| σφ | 49.6 mrad | 50 mrad |
| pz-sign flips | 6.2% (12% for \|pz\| < 10 MeV/c) | — |
| Speed | ~70 events/s, 1 CPU core | — |

The toy MC is optimistic: no multiple scattering, energy loss, tile planes,
fibre cross-talk or multi-fibre clusters. The comparison is for orientation,
not a validation.

## Live display look

Solid, lit (PBR) apparatus with image-based lighting and ACES tone mapping; a camera-facing **cutaway** (key `C`)
clips the half of every shell nearest the viewer so the CHeT interior and tracks stay visible; numbered
**callouts** (key `L`) open a description card per subsystem; an **explode** slider slides the end components
apart. All of it sits in the Run control panel under *Look*.

## Live display modes

* **decay** — one Michel positron per event, emitted from a muon already on the orbit.
* **μ⁺ injection** (key `I`) — the muon leaves the CW superconducting injection tube at (0, 47, −455) mm,
  spirals down the fringe field (cyclotron radius 34 → 30 mm, guiding centre drifting onto the axis, pitch
  shrinking to zero), is kicked at z = 0, circulates 3–4 turns on the 30 mm orbit with a small weakly-focusing
  bounce, then decays into the same positron event. An injection event lasts 1.9 × the selected interval.
  The spiral is a display model (no field map); its end point is the simulated decay vertex.

## GPU notes

* The GPU path uses the identical code; it **has not been run on a GPU in the
  environment where this was written** (no CUDA there). Run
  `pytest tests -k gpu` and `chet-track bench` on your machine first.
* Use `--float32` on GeForce/RTX cards (FP64 is 1/32–1/64 speed). On CPU,
  float32 gives the same efficiency and resolution and is ~1.5× faster.
* Batch size: `run(..., chunk=8192)` on a GPU; memory per sub-batch is
  bounded by `RecoConfig.work_budget` (≈ events × seeds × hits).
* The few host syncs left are size queries (`int(x.max())`) needed for padding.

## Things to set for the real detector

* `CHeTGeometry.stereo_deg` — **placeholder 10°**; use the as-built angles.
* Channel numbering — map your DAQ/Geant4 ids onto `geom.tables`.
* `SimConfig` noise, efficiency, jitter; `RecoConfig` tolerances.
* Cylinders only: the 30 orthogonal-fibre tiles in the 2025 design papers are not part of the as-built CHeT and are not modelled.

## Layout

```
chet_tracking/
  backend.py     numpy/cupy selection, timers, ragged→padded `compact`
  geometry.py    CHeT cylinders, stereo layers, SiPM channel map
  simulate.py    vectorised toy MC (frozen spin, Michel, helix, digitisation)
  hits.py        dead time, stereo u/v crossings
  finding.py     seeding + fit-based greedy assignment
  fitting.py     batched helix fit, kinematics, clone removal
  pipeline.py    reconstruct(), run(), evaluate()
  io.py          .npz/.csv/.root input
  cli.py         chet-track command
scripts/edm_updown.py   up/down asymmetry vs decay time demo
tests/                  unit + end-to-end tests (GPU test auto-skips)
```
