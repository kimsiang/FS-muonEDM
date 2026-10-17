import numpy as np
import pytest

from chet_tracking import CHeTGeometry, RecoConfig, SimConfig, evaluate, get_xp, run
from chet_tracking.backend import compact
from chet_tracking.fitting import fit_tracks
from chet_tracking.geometry import K_PT, digitize
from chet_tracking.hits import apply_dead_time, build_crossings

GEOM = CHeTGeometry()


def test_channel_map():
    t = GEOM.tables
    assert GEOM.n_channels == t["layer_nch"].sum()
    assert np.all(np.diff(t["ch_phi"][t["ch_layer"] == 0]) > 0)


def test_compact():
    xp = np
    m = np.array([[1, 0, 1, 1], [0, 0, 0, 1]], bool)
    a = np.arange(8).reshape(2, 4)
    out, v, c = compact(xp, m, dict(a=a))
    assert out["a"].tolist() == [[0, 2, 3], [7, 0, 0]]
    assert v.tolist() == [[True] * 3, [True, False, False]]


def test_stereo_round_trip():
    """Digitise random (phi, z) on every cylinder into u/v channels and invert."""
    xp = np
    gd = GEOM.device_tables(xp)
    rng = np.random.default_rng(0)
    n = 400
    cyl = rng.integers(0, GEOM.n_cyl, n)
    phi = rng.uniform(-np.pi, np.pi, n)
    z = rng.uniform(-190, 190, n)
    chu = digitize(xp, gd, 4, 2 * cyl, phi, z)
    chv = digitize(xp, gd, 4, 2 * cyl + 1, phi, z)
    ch = np.stack([chu, chv], 1)                       # one "event" per hit
    raw = dict(ch=ch, t=np.zeros((n, 2)), truth=np.zeros((n, 2), np.int32),
               valid=np.ones((n, 2), bool))
    pts, v = build_crossings(xp, raw, GEOM, gd)
    assert v[:, 0].all()
    # several 2*pi branches can lie inside |z| < L/2 (e.g. r = 20 mm, 10 deg,
    # |z| > 156 mm): the true one must be among the candidates
    dz_all = np.where(v, pts["z"] - z[:, None], np.inf)
    best = np.argmin(np.abs(dz_all), axis=1)
    dz = dz_all[np.arange(n), best]
    phir = np.arctan2(pts["y"], pts["x"])[np.arange(n), best]
    dphi = np.angle(np.exp(1j * (phir - phi)))
    sz = GEOM.tables["cyl_sigma_z"][cyl]
    assert np.std(dz / sz) < 1.5 and np.abs(dz).max() < 4 * sz.max()
    assert np.abs(dphi * GEOM.tables["cyl_radius"][cyl]).max() < 2.0


def test_dead_time_merges_same_channel():
    raw = dict(ch=np.array([[5, 5, 7, 5]]), t=np.array([[1.0, 3.0, 2.0, 30.0]]),
               truth=np.zeros((1, 4), np.int32), valid=np.ones((1, 4), bool))
    out = apply_dead_time(np, raw, 10.0)
    kept = sorted(zip(out["ch"][0][out["valid"][0]], out["t"][0][out["valid"][0]]))
    assert kept == [(5, 1.0), (5, 30.0), (7, 2.0)]


def test_fit_perfect_helix():
    """Exact points on a known helix (multi-turn) -> fitted p, theta."""
    xp = np
    gd = GEOM.device_tables(xp)
    R, h = 40.0, 12.0
    xc, yc = 25.0, -35.0
    psi0 = 0.7
    dpsi = np.array([0.3, 0.9, 1.5, 2.6, 6.6, 7.4])     # includes a second turn
    psi = psi0 - dpsi
    x, y = xc + R * np.cos(psi), yc + R * np.sin(psi)
    z = -20 + h * dpsi
    cand = dict(x=x[None], y=y[None], z=z[None], t=np.zeros((1, 6)), cyl=np.zeros((1, 6), np.int64),
                truth=np.zeros((1, 6), np.int32), hit_valid=np.ones((1, 6), bool),
                event=np.zeros(1, np.int64), seed_xc=np.array([xc + 1.5]), seed_yc=np.array([yc - 1]),
                seed_R=np.array([R + 2]), seed_h=np.array([h * 1.1]), seed_a=np.array([-20 + h * 0.3]),
                seed_x_ref=x[:1], seed_y_ref=y[:1])
    trk = fit_tracks(xp, cand, GEOM, gd, RecoConfig())
    pt = K_PT * GEOM.bfield_T * R
    assert trk["pt"][0] == pytest.approx(pt, rel=1e-6)
    assert trk["pz"][0] == pytest.approx(pt * h / R, rel=1e-6)


@pytest.mark.parametrize("dtype", ["float64", "float32"])
def test_end_to_end_cpu(dtype):
    tracks, truth, info = run(400, "cpu", seed=3, reco=RecoConfig(dtype=dtype), warmup=False)
    m = evaluate(tracks, truth)
    assert m["efficiency"] > 0.7
    assert m["fake_rate"] < 0.05
    assert m["sigma_p_over_p"] < 0.05          # requirement quoted for CHeT
    assert m["sigma_theta_mrad"] < 30


def test_cpu_gpu_agree():
    try:
        xp = get_xp("gpu")
    except RuntimeError:
        pytest.skip("no GPU / CuPy")
    from chet_tracking.backend import to_host
    from chet_tracking.pipeline import reconstruct
    from chet_tracking.simulate import simulate_events
    raw, _ = simulate_events(300, GEOM, np, 11, SimConfig())
    t_cpu = reconstruct(np, raw, GEOM)
    t_gpu = reconstruct(xp, {k: xp.asarray(v) for k, v in raw.items()}, GEOM)
    a = np.sort(t_cpu["p"])
    b = np.sort(to_host(t_gpu["p"]))
    # identical algorithm; tie-breaking in GPU sorts may differ for a few tracks
    assert abs(len(a) - len(b)) <= 0.02 * len(a) + 1
    if len(a) == len(b):
        assert np.mean(np.isclose(a, b, rtol=1e-5)) > 0.95
