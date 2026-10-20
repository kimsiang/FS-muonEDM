// CHeT tracking engine — JavaScript port of chet_tracking (single event at a time).
// Same chain as the Python package: toy MC -> dead time -> stereo crossings ->
// windowed triplet seeds (12 turn hypotheses) -> fit-based greedy assignment
// with raw-hit exclusivity -> circle + joint z/time-of-flight helix fit.
(function (root) {
  "use strict";
  const TWO_PI = 2 * Math.PI, C = 299.792458, KPT = 0.299792458;
  const M_MU = 105.6583755, M_E = 0.51099895;

  // ---------------------------------------------------------------- geometry
  function makeGeometry(opt = {}) {
    const g = Object.assign({ radii: [20, 23, 36, 45], stereo: [10, 10, 10, 10], pitch: 0.5, fpc: 4,
      L: 200, zRead: 200, B: 3, r0: 30, nIndex: 1.6 }, opt);
    g.vf = C / g.nIndex;
    g.layers = [];
    g.cyl = g.radii.map((r, c) => {
      const a = g.stereo[c] * Math.PI / 180;
      const nfib = Math.floor(TWO_PI * r * Math.cos(a) / g.pitch);
      const dphi = TWO_PI / nfib, nch = Math.ceil(nfib / g.fpc);
      const chPhi = [];
      for (let k = 0; k < nch; k++) chPhi.push((k * g.fpc + 0.5 * Math.min(g.fpc, nfib - k * g.fpc)) * dphi);
      const tan = Math.tan(a), cos = Math.cos(a);
      const sigZ = r * dphi * g.fpc * Math.sqrt(2 / 12) / (2 * tan);
      return { r, tan, cos, nfib, dphi, nch, chPhi, sigZ };
    });
    g.nChannels = g.cyl.reduce((s, c) => s + 2 * c.nch, 0);
    return g;
  }

  // ------------------------------------------------------------------ random
  function rng(seed) {             // mulberry32
    let s = seed >>> 0;
    const u = () => { s = (s + 0x6D2B79F5) >>> 0; let t = s; t = Math.imul(t ^ (t >>> 15), t | 1);
      t ^= t + Math.imul(t ^ (t >>> 7), t | 61); return ((t ^ (t >>> 14)) >>> 0) / 4294967296; };
    const n = () => Math.sqrt(-2 * Math.log(1 - u())) * Math.cos(TWO_PI * u());
    const poisson = (lam) => { let L = Math.exp(-lam), k = 0, p = 1; do { k++; p *= u(); } while (p > L); return k - 1; };
    return { u, n, poisson };
  }
  const mod2pi = (x) => ((x % TWO_PI) + TWO_PI) % TWO_PI;

  // -------------------------------------------------------------- simulation
  function simulate(g, R, opt = {}) {
    const o = Object.assign({ mult: 1, window: 50, pmu: 28, pol: 0.95, eff: 0.95, jitter: 0.5,
      dark: 55e3, maxTurns: 3, dead: 10, sigR: 0.5, sigZ: 5 }, opt);
    const eMu = Math.hypot(o.pmu, M_MU), gam = eMu / M_MU, bet = o.pmu / eMu;
    const W = (M_MU * M_MU + M_E * M_E) / (2 * M_MU);
    const tracks = [], hits = [];
    let hid = 0;
    for (let m = 0; m < o.mult; m++) {
      const phMu = TWO_PI * R.u(), rMu = g.r0 + o.sigR * R.n();
      const vx = rMu * Math.cos(phMu), vy = rMu * Math.sin(phMu), vz = o.sigZ * R.n();
      const nx = Math.sin(phMu), ny = -Math.cos(phMu);                 // mu+ clockwise
      const sx = -nx, sy = -ny, sz = 0;                                 // spin opposite momentum
      // Michel energy (bisection of 2x^3 - x^4 = u) and spin-correlated angle
      const uu = R.u(); let lo = 0, hi = 1;
      for (let i = 0; i < 40; i++) { const mid = (lo + hi) / 2; (2 * mid ** 3 - mid ** 4 < uu) ? lo = mid : hi = mid; }
      const x = (lo + hi) / 2, eR = Math.max(x * W, M_E * 1.0001), pR = Math.sqrt(eR * eR - M_E * M_E);
      const A = o.pol * (2 * x - 1) / (3 - 2 * x), u2 = R.u();
      let c = Math.abs(A) < 1e-6 ? 2 * u2 - 1 : (-1 + Math.sqrt(Math.max(0, 1 - A * (2 - A - 4 * u2)))) / A;
      c = Math.max(-1, Math.min(1, c));
      const om = TWO_PI * R.u(), st = Math.sqrt(1 - c * c);
      // basis perpendicular to s (s lies in xy plane): e1 = z x s, e2 = s x e1
      let e1x = -sy, e1y = sx, e1z = 0; const nn = Math.hypot(e1x, e1y); e1x /= nn; e1y /= nn;
      const e2x = sy * e1z - sz * e1y, e2y = sz * e1x - sx * e1z, e2z = sx * e1y - sy * e1x;
      let px = pR * (c * sx + st * (Math.cos(om) * e1x + Math.sin(om) * e2x));
      let py = pR * (c * sy + st * (Math.cos(om) * e1y + Math.sin(om) * e2y));
      const pz = pR * (c * sz + st * (Math.cos(om) * e1z + Math.sin(om) * e2z));
      const ppar = px * nx + py * ny, coef = (gam - 1) * ppar + gam * bet * eR;
      px += coef * nx; py += coef * ny;
      const eL = gam * (eR + bet * ppar);
      const pt = Math.max(Math.hypot(px, py), 1e-6), p = Math.hypot(pt, pz), beta = p / eL;
      const Rh = pt / (KPT * g.B), phiP = Math.atan2(py, px);
      const cx = vx + Rh * Math.sin(phiP), cy = vy - Rh * Math.cos(phiP), h = Rh * pz / pt;
      const psi0 = Math.atan2(vy - cy, vx - cx), t0 = o.window * R.u();
      const d = Math.hypot(cx, cy), bc = Math.atan2(cy, cx);
      let ncross = 0;
      const crossings = [];
      g.cyl.forEach((cy_, ci) => {
        const ca = (cy_.r ** 2 - d * d - Rh * Rh) / (2 * Rh * d);
        if (Math.abs(ca) > 1) return;
        for (const sgn of [1, -1]) {
          const psi = bc + sgn * Math.acos(ca), dt0 = mod2pi(psi0 - psi);
          for (let n = 0; n < o.maxTurns; n++) {
            const D = dt0 + TWO_PI * n, z = vz + h * D;
            if (Math.abs(z) > g.L) continue;
            const X = cx + Rh * Math.cos(psi), Y = cy + Rh * Math.sin(psi), phi = Math.atan2(Y, X);
            const t = t0 + D * Rh * p / pt / (beta * C);
            crossings.push({ x: X, y: Y, z, t, cyl: ci, D }); ncross++;
            for (let s = 0; s < 2; s++) {
              if (R.u() > o.eff) continue;
              const tn = s === 0 ? cy_.tan : -cy_.tan;
              const fib = Math.min(Math.floor(mod2pi(phi - z * tn / cy_.r) / cy_.dphi), cy_.nfib - 1);
              hits.push({ cyl: ci, s, ch: Math.floor(fib / g.fpc), truth: m, hid, D,
                t: t + (g.zRead - z) / (cy_.cos * g.vf) + o.jitter * R.n() });
            }
            hid++;
          }
        }
      });
      tracks.push({ p, pt, pz, theta: Math.atan2(pt, pz), phi: phiP, vx, vy, vz, cx, cy, R: Rh, h, psi0, t0,
        ncross, crossings });
    }
    // SiPM dark counts
    const nNoise = R.poisson(o.dark * (o.window + 10) * 1e-9 * g.nChannels);
    for (let i = 0; i < nNoise; i++) {
      let k = Math.floor(R.u() * g.nChannels), ci = 0;
      while (k >= 2 * g.cyl[ci].nch) { k -= 2 * g.cyl[ci].nch; ci++; }
      const s = k >= g.cyl[ci].nch ? 1 : 0;
      hits.push({ cyl: ci, s, ch: k - s * g.cyl[ci].nch, truth: -1, hid: -1, D: -1, t: (o.window + 10) * R.u() });
    }
    return { tracks, raw: deadTime(hits, o.dead), nNoise };
  }

  function deadTime(hits, dead) {
    const sorted = hits.slice().sort((a, b) => (a.cyl - b.cyl) || (a.s - b.s) || (a.ch - b.ch) || (a.t - b.t));
    const out = [];
    for (let i = 0; i < sorted.length; i++) {
      const a = sorted[i], b = sorted[i - 1];
      if (b && b.cyl === a.cyl && b.s === a.s && b.ch === a.ch && a.t - b.t < dead) continue;
      out.push(a);
    }
    out.forEach((h, i) => (h.idx = i));
    return out;
  }

  // ----------------------------------------------------------- reconstruction
  const CFG = { dtCross: 2, seedDt: 6, trackDt: 10, tolXY: 2, tolZ: 8, seedZTol: 8, sZ: 3, sT0: 0.6,
    rMin: 5, rMax: 95, orbitTol: 6, minHits: 4, maxTracks: 3, maxTurn: 4, window: 10, K: 6,
    sigXY: 0.8, sigT: 0.4, nsig: 4, chiPen: 2 };

  function crossingsOf(g, raw) {
    const pts = [];
    const us = raw.filter((h) => h.s === 0), vs = raw.filter((h) => h.s === 1);
    for (const u of us) for (const v of vs) {
      if (u.cyl !== v.cyl || Math.abs(u.t - v.t) >= CFG.dtCross) continue;
      const c = g.cyl[u.cyl], pu = c.chPhi[u.ch], pv = c.chPhi[v.ch];
      for (let k = -2; k <= 2; k++) {
        const z = (pv - pu + TWO_PI * k) * c.r / (2 * c.tan);
        if (Math.abs(z) > g.L) continue;
        const phi = pu + z * c.tan / c.r;
        pts.push({ x: c.r * Math.cos(phi), y: c.r * Math.sin(phi), z, cyl: u.cyl,
          t: 0.5 * (u.t + v.t) - (g.zRead - z) / (c.cos * g.vf),
          truth: u.truth === v.truth ? u.truth : -1, ghost: u.hid !== v.hid, hu: u.idx, hv: v.idx });
      }
    }
    pts.sort((a, b) => a.t - b.t);
    pts.forEach((p, i) => (p.i = i));
    return pts;
  }

  function circle3(a, b, c) {
    const d = 2 * (a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y));
    if (Math.abs(d) < 1e-9) return null;
    const s1 = a.x * a.x + a.y * a.y, s2 = b.x * b.x + b.y * b.y, s3 = c.x * c.x + c.y * c.y;
    const xc = (s1 * (b.y - c.y) + s2 * (c.y - a.y) + s3 * (a.y - b.y)) / d;
    const yc = (s1 * (c.x - b.x) + s2 * (a.x - c.x) + s3 * (b.x - a.x)) / d;
    return { xc, yc, R: Math.hypot(a.x - xc, a.y - yc) };
  }

  function seeds(g, pts) {
    const out = [], W = CFG.window;
    for (let i = 0; i < pts.length; i++) for (let j = i + 1; j <= i + W && j < pts.length; j++)
      for (let k = j + 1; k <= i + W && k < pts.length; k++) {
        const P3 = [pts[i], pts[j], pts[k]];
        if (P3[0].cyl === P3[1].cyl && P3[1].cyl === P3[2].cyl) continue;
        if (new Set(P3.map((p) => p.hu)).size < 3 || new Set(P3.map((p) => p.hv)).size < 3) continue;
        if (P3[2].t - P3[0].t >= CFG.seedDt) continue;
        const cc = circle3(...P3);
        if (!cc || cc.R <= CFG.rMin || cc.R >= CFG.rMax) continue;
        const dc = Math.hypot(cc.xc, cc.yc);
        if (!(Math.abs(dc - cc.R) < g.r0 + CFG.orbitTol && dc + cc.R > g.r0 - CFG.orbitTol)) continue;
        const psi = P3.map((p) => Math.atan2(p.y - cc.yc, p.x - cc.xc));
        let best = null;
        for (let r = 0; r < 3; r++) for (let na = 0; na < 2; na++) for (let nb = 0; nb < 2; nb++) {
          const oth = [0, 1, 2].filter((q) => q !== r), sh = [0, 0, 0];
          sh[oth[0]] = na; sh[oth[1]] = nb;
          const D = psi.map((ps, q) => mod2pi(psi[r] - ps) + TWO_PI * sh[q]);
          const md = (D[0] + D[1] + D[2]) / 3, mz = (P3[0].z + P3[1].z + P3[2].z) / 3;
          let sdd = 0, sdz = 0;
          for (let q = 0; q < 3; q++) { sdd += (D[q] - md) ** 2; sdz += (D[q] - md) * (P3[q].z - mz); }
          if (sdd <= 1e-4) continue;
          const h = sdz / sdd, a = mz - h * md;
          const zres = Math.max(...P3.map((p, q) => Math.abs(p.z - a - h * D[q])));
          if (zres >= CFG.seedZTol) continue;
          const L = Math.sqrt(cc.R ** 2 + h * h) / C;
          const tr = P3.map((p, q) => p.t - L * D[q]), mt = (tr[0] + tr[1] + tr[2]) / 3;
          const chi = (zres / CFG.sZ) ** 2 + tr.reduce((s, v) => s + (v - mt) ** 2, 0) / CFG.sT0 ** 2;
          if (!best || chi < best.chi) best = { chi, h, a, r };
        }
        if (!best) continue;
        const ref = P3[best.r];
        out.push({ xc: cc.xc, yc: cc.yc, R: cc.R, h: best.h, a: best.a, psiRef: psi[best.r],
          xRef: ref.x, yRef: ref.y, tRef: P3[0].t, idx: [i, j, k] });
      }
    return out;
  }

  function residual(s, p) {
    const dx = p.x - s.xc, dy = p.y - s.yc;
    const rxy = Math.abs(Math.hypot(dx, dy) - s.R);
    const D = mod2pi(s.psiRef - Math.atan2(dy, dx)), pitch = TWO_PI * s.h;
    let n = Math.abs(pitch) > 1e-3 ? Math.round((p.z - s.a - s.h * D) / pitch) : 0;
    n = Math.max(-1, Math.min(CFG.maxTurn, n));
    const rz = Math.abs(p.z - s.a - s.h * (D + TWO_PI * n)), dt = p.t - s.tRef;
    return { ok: rxy < CFG.tolXY && rz < CFG.tolZ && dt > -CFG.seedDt && dt < CFG.trackDt,
      chi: (rxy / CFG.sigXY) ** 2 + (rz / (0.5 * CFG.tolZ)) ** 2 };
  }

  function solve3(A, b) {   // Gaussian elimination with partial pivoting
    const M = A.map((r, i) => [...r, b[i]]);
    for (let c = 0; c < 3; c++) {
      let piv = c; for (let r = c + 1; r < 3; r++) if (Math.abs(M[r][c]) > Math.abs(M[piv][c])) piv = r;
      [M[c], M[piv]] = [M[piv], M[c]];
      if (Math.abs(M[c][c]) < 1e-15) return [0, 0, 0];
      for (let r = 0; r < 3; r++) if (r !== c) { const f = M[r][c] / M[c][c]; for (let k = c; k < 4; k++) M[r][k] -= f * M[c][k]; }
    }
    return [M[0][3] / M[0][0], M[1][3] / M[1][1], M[2][3] / M[2][2]];
  }

  function fit(g, P, seed) {
    let w = P.map(() => 1), { xc, yc, R, h, a } = seed;
    const sz = P.map((p) => g.cyl[p.cyl].sigZ), st = CFG.sigT;
    let U = [], rz = [], rt = [], rxy = [], t0 = 0;
    for (let pass = 0; pass < 3; pass++) {
      for (let it = 0; it < 5; it++) {           // Gauss-Newton circle fit
        const JTJ = [[0, 0, 0], [0, 0, 0], [0, 0, 0]], JTr = [0, 0, 0];
        P.forEach((p, i) => {
          if (!w[i]) return;
          const dx = p.x - xc, dy = p.y - yc, rho = Math.max(Math.hypot(dx, dy), 1e-9);
          const J = [-dx / rho, -dy / rho, -1], res = rho - R;
          for (let m = 0; m < 3; m++) { JTr[m] += J[m] * res; for (let n = 0; n < 3; n++) JTJ[m][n] += J[m] * J[n]; }
        });
        const tr = JTJ[0][0] + JTJ[1][1] + JTJ[2][2];
        for (let m = 0; m < 3; m++) JTJ[m][m] += 1e-6 * tr + 1e-9;
        const dlt = solve3(JTJ, JTr.map((v) => -v));
        xc += dlt[0]; yc += dlt[1]; R = Math.abs(R + dlt[2]);
      }
      const psi = P.map((p) => Math.atan2(p.y - yc, p.x - xc));
      const psiRef = Math.atan2(seed.yRef - yc, seed.xRef - xc);
      const D = psi.map((ps) => mod2pi(psiRef - ps));
      let best = null;
      for (const sign of [1, -1]) {               // up/down hypotheses
        let hh = sign * h, aa = a, L = Math.sqrt(R * R + hh * hh) / C, UU = D.slice();
        const tw = () => { let s = 0, n = 0; P.forEach((p, i) => { if (w[i]) { s += p.t - L * UU[i]; n++; } }); return s / Math.max(n, 1); };
        let tt = tw();
        for (let it = 0; it < 4; it++) {
          UU = P.map((p, i) => {
            let bu = D[i], bc = Infinity;
            for (let n = -1; n <= CFG.maxTurn; n++) {
              const u = D[i] + TWO_PI * n;
              const c = ((p.z - aa - hh * u) / sz[i]) ** 2 + ((p.t - tt - L * u) / st) ** 2;
              if (c < bc) { bc = c; bu = u; }
            }
            return bu;
          });
          let sw = 0, su = 0, szz = 0, suu = 0, suz = 0;
          P.forEach((p, i) => { if (!w[i]) return; const q = 1 / sz[i] ** 2;
            sw += q; su += q * UU[i]; szz += q * p.z; suu += q * UU[i] ** 2; suz += q * UU[i] * p.z; });
          const det = sw * suu - su * su;
          if (det > 1e-9 * sw * suu) { hh = (sw * suz - su * szz) / det; aa = (suu * szz - su * suz) / det; }
          L = Math.sqrt(R * R + hh * hh) / C; tt = tw();
        }
        const RZ = P.map((p, i) => (p.z - aa - hh * UU[i]) / sz[i]);
        const RT = P.map((p, i) => (p.t - tt - L * UU[i]) / st);
        const chi2 = P.reduce((s, p, i) => s + w[i] * (RZ[i] ** 2 + RT[i] ** 2), 0);
        if (!best || chi2 < best.chi2) best = { h: hh, a: aa, t0: tt, U: UU, RZ, RT, chi2 };
      }
      ({ h, a, t0, U } = best); rz = best.RZ; rt = best.RT;
      rxy = P.map((p) => (Math.hypot(p.x - xc, p.y - yc) - R) / CFG.sigXY);
      const wn = w.map((wi, i) => wi && Math.abs(rxy[i]) < CFG.nsig && Math.abs(rz[i]) < CFG.nsig && Math.abs(rt[i]) < CFG.nsig ? 1 : 0);
      if (wn.reduce((s, v) => s + v, 0) >= 3) w = wn;
    }
    const nh = w.reduce((s, v) => s + v, 0);
    const chi2 = P.reduce((s, p, i) => s + w[i] * (rxy[i] ** 2 + rz[i] ** 2 + rt[i] ** 2), 0);
    const ndf = Math.max(3 * nh - 6, 1);
    // vertex on the muon orbit just before the first hit
    let fi = -1; P.forEach((p, i) => { if (w[i] && (fi < 0 || U[i] < U[fi])) fi = i; });
    const psiF = Math.atan2(P[fi].y - yc, P[fi].x - xc), d = Math.max(Math.hypot(xc, yc), 1e-9), bc = Math.atan2(yc, xc);
    const ac = Math.acos(Math.max(-1, Math.min(1, (g.r0 ** 2 - d * d - R * R) / (2 * R * d))));
    const cand = [bc + ac, bc - ac].map((ps) => ({ ps, dl: mod2pi(ps - psiF) })).sort((x, y) => x.dl - y.dl)[0];
    const pt = KPT * g.B * R, pz = pt * h / R;
    return { xc, yc, R, h, a, t0, U, w, nh, chi2ndf: chi2 / ndf, pt, pz, p: Math.hypot(pt, pz),
      theta: Math.atan2(pt, pz), phi: Math.atan2(-Math.cos(cand.ps), Math.sin(cand.ps)),
      vx: xc + R * Math.cos(cand.ps), vy: yc + R * Math.sin(cand.ps), vz: a + h * U[fi] - h * cand.dl,
      Uv: U[fi] - cand.dl };
  }

  function reconstruct(g, raw) {
    const t_start = (typeof performance !== "undefined" ? performance : Date).now();
    const pts = crossingsOf(g, raw);
    const S = seeds(g, pts);
    const comp = S.map((s) => pts.map((p) => residual(s, p)));
    const free = pts.map(() => true), tracks = [];
    for (let it = 0; it < CFG.maxTracks; it++) {
      const scored = S.map((s, si) => {
        let n = 0, c = 0; comp[si].forEach((r, pi) => { if (r.ok && free[pi]) { n++; c += r.chi; } });
        return { si, score: n - 1e-3 * c, n };
      }).filter((x) => x.n >= CFG.minHits).sort((a, b) => b.score - a.score).slice(0, CFG.K);
      let best = null;
      for (const { si } of scored) {
        let sel = pts.filter((p, pi) => comp[si][pi].ok && free[pi]);
        // two points sharing a raw hit: keep the better one
        sel = sel.filter((p) => !sel.some((q) => q !== p && (q.hu === p.hu || q.hv === p.hv) &&
          (comp[si][q.i].chi < comp[si][p.i].chi || (comp[si][q.i].chi === comp[si][p.i].chi && q.i < p.i))));
        if (sel.length < CFG.minHits) continue;
        const f = fit(g, sel, S[si]);
        const q = f.nh - CFG.chiPen * f.chi2ndf;
        if (f.nh >= CFG.minHits && (!best || q > best.q)) best = { q, f, sel, seed: S[si] };
      }
      if (!best) break;
      const inl = best.sel.filter((p, i) => best.f.w[i]);
      const used = new Set(); inl.forEach((p) => { used.add("u" + p.hu); used.add("v" + p.hv); });
      pts.forEach((p, pi) => { if (used.has("u" + p.hu) || used.has("v" + p.hv)) free[pi] = false; });
      const counts = {}; inl.forEach((p) => (counts[p.truth] = (counts[p.truth] || 0) + 1));
      const [lab, cnt] = Object.entries(counts).sort((a, b) => b[1] - a[1])[0];
      tracks.push(Object.assign(best.f, { hits: inl, all: best.sel, seed: best.seed,
        truth: +lab, purity: cnt / inl.length, ghostFrac: inl.filter((p) => p.ghost).length / inl.length }));
    }
    const ms = (typeof performance !== "undefined" ? performance : Date).now() - t_start;
    return { pts, nSeeds: S.length, tracks, ms };
  }

  root.CHeT = { makeGeometry, rng, simulate, reconstruct, CFG, TWO_PI, mod2pi };
  if (typeof module !== "undefined") module.exports = root.CHeT;
})(typeof window !== "undefined" ? window : globalThis);
