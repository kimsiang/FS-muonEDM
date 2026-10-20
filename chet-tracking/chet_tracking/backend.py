"""CPU / GPU array backend.

Every algorithm in this package is written once against the NumPy API and
receives the array module ``xp`` as an argument.  ``xp`` is either ``numpy``
(CPU) or ``cupy`` (CUDA GPU); the code paths are identical.
"""
from __future__ import annotations

import time
import warnings
from collections import defaultdict
from contextlib import contextmanager

import numpy as np


def get_xp(device: str = "cpu"):
    """Return the array module for ``device`` in {"cpu", "gpu", "cuda", "auto"}."""
    device = device.lower()
    if device == "cpu":
        return np
    if device in ("gpu", "cuda", "auto"):
        try:
            import cupy as cp

            cp.cuda.runtime.getDeviceCount()
            return cp
        except Exception as exc:  # pragma: no cover - depends on hardware
            if device == "auto":
                warnings.warn(f"GPU unavailable ({exc!s}); falling back to CPU.")
                return np
            raise RuntimeError(
                "GPU backend requested but CuPy/CUDA is not usable: "
                f"{exc!s}. Install e.g. `pip install cupy-cuda12x`."
            ) from exc
    raise ValueError(f"unknown device {device!r}")


def is_gpu(xp) -> bool:
    return xp is not np


def device_name(xp) -> str:
    if not is_gpu(xp):
        return "cpu (numpy)"
    props = xp.cuda.runtime.getDeviceProperties(xp.cuda.Device().id)
    name = props["name"]
    return f"gpu ({name.decode() if isinstance(name, bytes) else name})"


def to_host(a):
    """Device array -> numpy array (no-op on CPU)."""
    if hasattr(a, "get"):
        return a.get()
    return np.asarray(a)


def to_device(xp, a):
    return xp.asarray(a)


def synchronize(xp) -> None:
    if is_gpu(xp):
        xp.cuda.Stream.null.synchronize()


class StageTimer:
    """Wall-clock timer per pipeline stage, synchronising the GPU stream."""

    def __init__(self, xp):
        self.xp = xp
        self.totals = defaultdict(float)

    @contextmanager
    def __call__(self, stage: str):
        synchronize(self.xp)
        t0 = time.perf_counter()
        yield
        synchronize(self.xp)
        self.totals[stage] += time.perf_counter() - t0

    def as_dict(self):
        return dict(self.totals)


def compact(xp, mask, arrays: dict, max_len: int | None = None):
    """Left-pack the True entries of ``mask`` (shape [B, ...]) row by row.

    ``arrays`` maps names to arrays broadcastable to ``mask.shape``.
    Returns (packed dict of [B, N] arrays, valid [B, N] bool, counts [B]).
    This is the core ragged-to-padded primitive used on both CPU and GPU.
    """
    B = mask.shape[0]
    flat = mask.reshape(B, -1)
    counts = flat.sum(axis=1)
    n = int(counts.max()) if B > 0 and flat.shape[1] > 0 else 0
    if max_len is not None:
        n = min(n, int(max_len))
    n = max(n, 1)
    pos = xp.cumsum(flat, axis=1) - 1
    keep = flat & (pos < n)
    b, i = xp.nonzero(keep)
    p = pos[b, i]
    out = {}
    for key, a in arrays.items():
        a = xp.broadcast_to(a, mask.shape).reshape(B, -1)
        o = xp.zeros((B, n), dtype=a.dtype)
        o[b, p] = a[b, i]
        out[key] = o
    valid = xp.zeros((B, n), dtype=bool)
    valid[b, p] = True
    return out, valid, counts
