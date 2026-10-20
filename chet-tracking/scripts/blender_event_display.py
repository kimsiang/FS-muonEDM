"""Render a CHeT event display with Blender (bpy).

Runs the chet_tracking reconstruction on toy-MC events, picks one clean curling
positron, and builds the scene: four cylinders of stereo scintillating fibres
(every fibre drawn as a true helix on its cylinder), the fibres that fired,
the reconstructed space points, the fitted helix and the MC-truth helix, the
30 mm muon orbit.  Rendered with Cycles.

    pip install bpy                      # Blender as a Python module
    python scripts/blender_event_display.py --out docs/chet_event_blender.png
    python scripts/blender_event_display.py --samples 256 --res 2000 --blend scene.blend

Units in the scene: 1 Blender unit = 10 mm.
"""
from __future__ import annotations

import argparse
import math

import numpy as np

from chet_tracking import CHeTGeometry, RecoConfig, SimConfig, simulate_events
from chet_tracking.finding import find_tracks
from chet_tracking.fitting import fit_tracks
from chet_tracking.hits import build_crossings

U = 0.1          # mm -> Blender units
NAVY = (0.0016, 0.011, 0.024)      # #051C2C in linear RGB
BLUE = (0.016, 0.071, 1.0)         # #2251FF
CYAN = (0.0, 0.40, 0.90)           # #00A9F4
INK = (0.82, 0.89, 0.94)
AMBER = (0.89, 0.46, 0.06)


# --------------------------------------------------------------------- event
def pick_event(geom, seed0=1, tries=40):
    """First event with one clean, well-measured curling positron."""
    xp, reco = np, RecoConfig()
    gd = geom.device_tables(xp)
    for s in range(seed0, seed0 + tries):
        raw, tru = simulate_events(64, geom, xp, s, SimConfig(mean_multiplicity=0.01))
        pts, pv = build_crossings(xp, raw, geom, gd, reco.crossing_dt_ns)
        cand = find_tracks(xp, pts, pv, geom, reco)
        if cand is None:
            continue
        fit = fit_tracks(xp, cand, geom, gd, reco)
        for i in range(len(fit["p"])):
            e = int(fit["event"][i])
            pz = tru["pz"][e, 0]
            if (fit["truth"][i] == 0 and fit["purity"][i] > 0.95 and fit["ghost_frac"][i] < 0.2
                    and 10 <= fit["nhits"][i] and 6 < abs(pz) < 20 and tru["p"][e, 0] > 30
                    and np.sign(fit["pz"][i]) == np.sign(pz)
                    and abs(fit["p"][i] / tru["p"][e, 0] - 1) < 0.03):
                hv = cand["hit_valid"][i] & fit["hit_w"][i]
                return dict(
                    raw={k: v[e][raw["valid"][e]] for k, v in raw.items() if k != "valid"},
                    pts={k: v[e][pv[e]] for k, v in pts.items()},
                    hits=np.stack([cand["x"][i][hv], cand["y"][i][hv], cand["z"][i][hv]], 1),
                    fit={k: float(fit[k][i]) for k in ("xc", "yc", "R", "h", "vx", "vy", "vz", "p", "pt", "pz", "theta")},
                    truth={k: float(tru[k][e, 0]) for k in ("p", "pt", "pz", "theta", "phi", "vx", "vy", "vz")},
                    seed=s, event=e)
    raise RuntimeError("no suitable event found")


def helix_points(xc, yc, R, h, vx, vy, vz, L, n=600, max_turn=6):
    """Points along a mu+-decay positron helix (clockwise) from the vertex to |z| = L."""
    psi0 = math.atan2(vy - yc, vx - xc)
    out = []
    for k in range(n + 1):
        u = 2 * math.pi * max_turn * k / n
        z = vz + h * u
        if abs(z) > L:
            break
        out.append((xc + R * math.cos(psi0 - u), yc + R * math.sin(psi0 - u), z))
    return out


# --------------------------------------------------------------------- blender
def build_scene(ev, geom, args):
    import bpy

    bpy.ops.wm.read_factory_settings(use_empty=True)
    sc = bpy.context.scene

    def mat(name, color, emit=0.0, alpha=1.0, rough=0.5):
        m = bpy.data.materials.new(name)
        m.use_nodes = True
        nt = m.node_tree
        bsdf = nt.nodes["Principled BSDF"]
        bsdf.inputs["Base Color"].default_value = (*color, 1)
        bsdf.inputs["Roughness"].default_value = rough
        bsdf.inputs["Alpha"].default_value = alpha
        if emit:
            bsdf.inputs["Emission Color"].default_value = (*color, 1)
            bsdf.inputs["Emission Strength"].default_value = emit
        if alpha < 1:
            try:
                m.surface_render_method = "BLENDED"
            except AttributeError:
                m.blend_method = "BLEND"
        return m

    def curve(name, splines, bevel, material, resolution=2):
        cu = bpy.data.curves.new(name, "CURVE")
        cu.dimensions = "3D"
        cu.bevel_depth = bevel
        cu.bevel_resolution = resolution
        for pts in splines:
            sp = cu.splines.new("POLY")
            sp.points.add(len(pts) - 1)
            for p, (x, y, z) in zip(sp.points, pts):
                p.co = (x * U, y * U, z * U, 1)
        ob = bpy.data.objects.new(name, cu)
        ob.data.materials.append(material)
        sc.collection.objects.link(ob)
        return ob

    m_fib = mat("fibre", (0.05, 0.13, 0.30), emit=0.55)
    m_fired = mat("fired", INK, emit=0.9)
    m_shell = mat("shell", (0.25, 0.40, 0.60), alpha=0.05, rough=0.2)
    m_track = mat("track", BLUE, emit=7.0)
    m_truth = mat("truth", CYAN, emit=2.5)
    m_hit = mat("hit", AMBER, emit=5.0)
    m_ghost = mat("ghost", (0.30, 0.39, 0.46), emit=0.6)
    m_orbit = mat("orbit", CYAN, emit=1.5)

    t = geom.tables
    L = geom.half_length_mm
    zs = np.linspace(-L, L, 48)
    fired = {(int(t["ch_layer"][c]), int(c - t["layer_offset"][t["ch_layer"][c]])) for c in ev["raw"]["ch"]}
    for c in range(geom.n_cyl):
        r, tan = t["cyl_radius"][c], t["cyl_tan"][c]
        for s, sgn in ((0, 1.0), (1, -1.0)):
            lay = 2 * c + s
            rr = r + (0.6 if s else 0.0)
            nfib, dphi = int(t["layer_nfib"][lay]), float(t["layer_dphi_fib"][lay])
            fpc = geom.fibers_per_channel
            bg, lit = [], []
            for f in range(0, nfib, args.fibre_stride):
                phi0 = (f + 0.5) * dphi
                # cut-away: keep fibres whose mid-point lies on the far half from the camera
                back = math.radians(args.azimuth) + math.pi
                if math.cos(phi0 - back) < args.cutaway:
                    continue
                pts = [(rr * math.cos(phi0 + sgn * z * tan / r), rr * math.sin(phi0 + sgn * z * tan / r), z) for z in zs]
                bg.append(pts)
            for (l2, ch) in fired:
                if l2 != lay:
                    continue
                for f in [min(ch * fpc + fpc // 2, nfib - 1)]:      # one fibre per fired SiPM
                    phi0 = (f + 0.5) * dphi
                    lit.append([(rr * math.cos(phi0 + sgn * z * tan / r), rr * math.sin(phi0 + sgn * z * tan / r), z) for z in zs])
            curve(f"fibres_c{c}_{'uv'[s]}", bg, 0.025 * 10 * U, m_fib, 1)
            if lit:
                curve(f"fired_c{c}_{'uv'[s]}", lit, 0.022 * 10 * U, m_fired, 1)
        # end rings mark each cylinder
        for zr in (-L, L):
            ringc = [(r * math.cos(a), r * math.sin(a), zr) for a in np.linspace(0, 2 * math.pi, 161)]
            curve(f"ring_c{c}_{zr:+.0f}", [ringc], 0.12 * U, m_fib)

    # muon orbit
    ring = [(geom.orbit_radius_mm * math.cos(a), geom.orbit_radius_mm * math.sin(a), 0.0)
            for a in np.linspace(0, 2 * math.pi, 181)]
    curve("muon_orbit", [ring], 0.25 * U, m_orbit)

    # tracks
    f, tr = ev["fit"], ev["truth"]
    curve("fitted_helix", [helix_points(f["xc"], f["yc"], f["R"], f["h"], f["vx"], f["vy"], f["vz"], L)], 0.55 * U, m_track, 3)
    Rt = tr["pt"] / (0.299792458 * geom.bfield_T)
    cxt, cyt = tr["vx"] + Rt * math.sin(tr["phi"]), tr["vy"] - Rt * math.cos(tr["phi"])
    curve("truth_helix", [helix_points(cxt, cyt, Rt, Rt * tr["pz"] / tr["pt"], tr["vx"], tr["vy"], tr["vz"], L)], 0.22 * U, m_truth)

    # space points: on-track amber, other (ghost) grey
    on = {tuple(np.round(p, 3)) for p in ev["hits"]}
    for x, y, z in zip(ev["pts"]["x"], ev["pts"]["y"], ev["pts"]["z"]):
        key = (round(float(x), 3), round(float(y), 3), round(float(z), 3))
        bpy.ops.mesh.primitive_uv_sphere_add(radius=(2.6 if key in on else 1.1) * U, location=(x * U, y * U, z * U),
                                             segments=16, ring_count=8)
        bpy.context.object.data.materials.append(m_hit if key in on else m_ghost)
    bpy.ops.mesh.primitive_uv_sphere_add(radius=1.6 * U, location=(f["vx"] * U, f["vy"] * U, f["vz"] * U))
    bpy.context.object.data.materials.append(m_orbit)

    # world, light, camera
    world = bpy.data.worlds.new("navy")
    world.use_nodes = True
    world.node_tree.nodes["Background"].inputs[0].default_value = (*NAVY, 1)
    world.node_tree.nodes["Background"].inputs[1].default_value = 1.0
    sc.world = world
    light = bpy.data.lights.new("key", "AREA")
    light.energy, light.size = 900, 8
    lo = bpy.data.objects.new("key", light)
    lo.location = (14, -10, 22)
    sc.collection.objects.link(lo)
    lo.rotation_euler = (math.radians(35), 0, math.radians(50))

    cam = bpy.data.cameras.new("cam")
    cam.lens = args.lens
    co = bpy.data.objects.new("cam", cam)
    sc.collection.objects.link(co)
    az, el, dist = math.radians(args.azimuth), math.radians(args.elevation), args.distance
    co.location = (dist * math.cos(el) * math.cos(az), dist * math.cos(el) * math.sin(az), dist * math.sin(el))
    target = bpy.data.objects.new("target", None)
    target.location = (0, 0, args.target_z * U)
    sc.collection.objects.link(target)
    tc = co.constraints.new("TRACK_TO")
    tc.target, tc.track_axis, tc.up_axis = target, "TRACK_NEGATIVE_Z", "UP_Y"
    sc.camera = co

    sc.render.engine = "CYCLES"
    sc.cycles.device = "CPU"
    sc.cycles.samples = args.samples
    try:
        sc.cycles.use_denoising = True
    except AttributeError:
        pass
    sc.render.resolution_x = int(args.res * 0.8)
    sc.render.resolution_y = args.res
    sc.render.film_transparent = False
    sc.view_settings.view_transform = "Standard"     # keep the palette's blues un-desaturated
    sc.render.filepath = args.out
    if args.blend:
        bpy.ops.wm.save_as_mainfile(filepath=args.blend)
    bpy.ops.render.render(write_still=True)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", default="docs/chet_event_blender.png")
    ap.add_argument("--blend", default=None, help="also save the .blend scene")
    ap.add_argument("--samples", type=int, default=128)
    ap.add_argument("--res", type=int, default=1500, help="image height (width = 0.8 x height)")
    ap.add_argument("--fibre-stride", type=int, default=4, help="draw every n-th background fibre")
    ap.add_argument("--cutaway", type=float, default=-0.15, help="draw fibres with cos(phi - far side) above this")
    ap.add_argument("--azimuth", type=float, default=-38.0)
    ap.add_argument("--elevation", type=float, default=38.0)
    ap.add_argument("--distance", type=float, default=80.0)
    ap.add_argument("--lens", type=float, default=68.0)
    ap.add_argument("--target-z", type=float, default=0.0)
    ap.add_argument("--seed", type=int, default=1)
    args = ap.parse_args()
    geom = CHeTGeometry()
    ev = pick_event(geom, args.seed)
    f, t = ev["fit"], ev["truth"]
    print(f"event seed {ev['seed']}/{ev['event']}: p = {f['p']:.1f} (true {t['p']:.1f}) MeV/c, "
          f"pz = {f['pz']:.1f} (true {t['pz']:.1f}), {len(ev['hits'])} hits, {len(ev['pts']['x'])} space points")
    build_scene(ev, geom, args)
    print("rendered", args.out)


if __name__ == "__main__":
    main()
