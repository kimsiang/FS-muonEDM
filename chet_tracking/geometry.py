"""CHeT (Cylindrical Helix Tracker) geometry and channel map.

Defaults follow A. Venturini et al., NIM A 1080 (2025) 170760 and the muEDM
Phase-I design paper (EPJC 85 (2025) 622):
  * 4 cylinders, axes || B, r = 20, 23, 36, 45 mm (muon orbit at 30 mm)
  * each cylinder = 2 stereo layers of 500 um square scintillating fibres
  * 4 adjacent fibres read out by one SiPM, single-ended readout downstream
  * active length 400 mm, solenoid field 3 T
The stereo angle is not quoted in those papers; 10 deg is a placeholder --
set it to the as-built value.

Conventions: mm, ns, MeV, T.  Layer index l = 2*c + s, s = 0 (u, +alpha),
s = 1 (v, -alpha).  A u-fibre with reference angle phi_u passes through
phi(z) = phi_u + z*tan(alpha)/r.
"""
from __future__ import annotations

import json
import math
from dataclasses import asdict, dataclass, field

import numpy as np

C_MM_PER_NS = 299.792458
K_PT = 0.299792458  # pT[MeV] = K_PT * B[T] * R[mm]
M_MU = 105.6583755
M_E = 0.51099895
TAU_MU_NS = 2196.9811


@dataclass
class CHeTGeometry:
    radii_mm: tuple = (20.0, 23.0, 36.0, 45.0)
    stereo_deg: tuple = (10.0, 10.0, 10.0, 10.0)
    fiber_pitch_mm: float = 0.5
    fibers_per_channel: int = 4
    half_length_mm: float = 200.0
    readout_z_mm: float = 200.0  # SiPMs at the downstream end
    bfield_T: float = 3.0
    orbit_radius_mm: float = 30.0
    fiber_refractive_index: float = 1.6
    tables: dict = field(default_factory=dict, repr=False, compare=False)

    def __post_init__(self):
        self.radii_mm = tuple(float(r) for r in self.radii_mm)
        self.stereo_deg = tuple(float(a) for a in self.stereo_deg)
        if len(self.radii_mm) != len(self.stereo_deg):
            raise ValueError("radii_mm and stereo_deg must have equal length")
        self._build_tables()

    # ------------------------------------------------------------------ build
    @property
    def n_cyl(self) -> int:
        return len(self.radii_mm)

    @property
    def n_layers(self) -> int:
        return 2 * self.n_cyl

    @property
    def v_fiber(self) -> float:
        return C_MM_PER_NS / self.fiber_refractive_index

    def _build_tables(self):
        fpc = self.fibers_per_channel
        lay_r, lay_tan, lay_cos, lay_nfib, lay_dphi, lay_nch, lay_off = ([] for _ in range(7))
        ch_layer, ch_phi = [], []
        off = 0
        for r, a in zip(self.radii_mm, self.stereo_deg):
            alpha = math.radians(a)
            nfib = int(math.floor(2 * math.pi * r * math.cos(alpha) / self.fiber_pitch_mm))
            dphi = 2 * math.pi / nfib
            nch = int(math.ceil(nfib / fpc))
            for s, sign in enumerate((+1.0, -1.0)):
                lay_r.append(r)
                lay_tan.append(sign * math.tan(alpha))
                lay_cos.append(math.cos(alpha))
                lay_nfib.append(nfib)
                lay_dphi.append(dphi)
                lay_nch.append(nch)
                lay_off.append(off)
                for g in range(nch):
                    first = g * fpc
                    width = min(fpc, nfib - first)
                    ch_layer.append(len(lay_r) - 1)
                    ch_phi.append((first + 0.5 * width) * dphi)
                off += nch
        cyl_r = np.array(self.radii_mm)
        cyl_tan = np.tan(np.radians(self.stereo_deg))
        ch_dphi = np.array(lay_dphi[::2]) * fpc
        # z resolution of a u/v crossing: Var(phi_v - phi_u) = 2 * (dphi_ch^2/12)
        cyl_sigma_z = cyl_r * ch_dphi * math.sqrt(2.0 / 12.0) / (2 * cyl_tan)
        cyl_sigma_t = cyl_r * ch_dphi / math.sqrt(12.0) / math.sqrt(2.0)
        self.tables = dict(
            layer_radius=np.array(lay_r),
            layer_tan=np.array(lay_tan),
            layer_cos=np.array(lay_cos),
            layer_nfib=np.array(lay_nfib, dtype=np.int64),
            layer_dphi_fib=np.array(lay_dphi),
            layer_nch=np.array(lay_nch, dtype=np.int64),
            layer_offset=np.array(lay_off, dtype=np.int64),
            ch_layer=np.array(ch_layer, dtype=np.int64),
            ch_phi=np.array(ch_phi),
            cyl_radius=cyl_r,
            cyl_tan=cyl_tan,
            cyl_cos=np.cos(np.radians(self.stereo_deg)),
            cyl_sigma_z=cyl_sigma_z,
            cyl_sigma_rphi=cyl_sigma_t,
        )

    @property
    def n_channels(self) -> int:
        return int(self.tables["ch_layer"].size)

    def device_tables(self, xp) -> dict:
        """Geometry lookup tables as arrays on the chosen device."""
        return {k: xp.asarray(v) for k, v in self.tables.items()}

    # ------------------------------------------------------------ utilities
    def max_stereo_wraps(self) -> int:
        """How many 2*pi branches k must be tried when inverting u/v -> z."""
        span = 2 * self.half_length_mm * self.tables["cyl_tan"] / self.tables["cyl_radius"]
        return int(math.ceil(float(span.max()) / (2 * math.pi))) + 1

    def summary(self) -> str:
        t = self.tables
        lines = [f"CHeT: {self.n_cyl} cylinders, {self.n_channels} SiPM channels, "
                 f"B = {self.bfield_T} T, |z| <= {self.half_length_mm} mm"]
        for c in range(self.n_cyl):
            lines.append(
                f"  cyl {c}: r = {t['cyl_radius'][c]:5.1f} mm, stereo = +-{self.stereo_deg[c]:.1f} deg, "
                f"{t['layer_nfib'][2*c]} fibres/layer, {t['layer_nch'][2*c]} ch/layer, "
                f"sigma_z ~ {t['cyl_sigma_z'][c]:.2f} mm"
            )
        return "\n".join(lines)

    def to_json(self) -> str:
        d = asdict(self)
        d.pop("tables")
        return json.dumps(d, indent=2)

    @classmethod
    def from_json(cls, text_or_path: str) -> "CHeTGeometry":
        try:
            d = json.loads(text_or_path)
        except json.JSONDecodeError:
            with open(text_or_path) as f:
                d = json.load(f)
        return cls(**d)


def digitize(xp, gd: dict, fibers_per_channel: int, layer, phi, z):
    """Hit at (phi, z) in stereo ``layer`` (array of layer ids) -> global channel id."""
    r = gd["layer_radius"][layer]
    tan = gd["layer_tan"][layer]
    dphi = gd["layer_dphi_fib"][layer]
    nfib = gd["layer_nfib"][layer]
    phi_ref = xp.mod(phi - z * tan / r, 2 * np.pi)
    fib = xp.minimum(xp.floor(phi_ref / dphi).astype(xp.int64), nfib - 1)
    return gd["layer_offset"][layer] + fib // fibers_per_channel
