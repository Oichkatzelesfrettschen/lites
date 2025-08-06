"""Tests for the :mod:`trace_graph` utility."""

from __future__ import annotations

import subprocess
from pathlib import Path
import sys


def test_trace_graph_generates_output(tmp_path: Path) -> None:
    """Ensure the utility produces a DOT file capturing include edges.

    @param tmp_path  pytest-provided temporary directory for test files.
    """
    a = tmp_path / "a_trace.c"
    a.write_text('#include "b_trace.h"\n')
    b = tmp_path / "b_trace.h"
    b.write_text("// trace header\n")
    script = Path(__file__).resolve().parents[1] / "scripts" / "trace_graph.py"
    output = tmp_path / "graph.dot"
    listing = tmp_path / "files.txt"
    subprocess.run(
        [
            sys.executable,
            str(script),
            "-r",
            str(tmp_path),
            "-o",
            str(output),
            "--files",
            str(listing),
        ],
        check=True,
    )
    assert output.exists() and listing.exists()
    dot_contents = output.read_text()
    listing_contents = listing.read_text()
    assert "a_trace.c" in dot_contents and "b_trace.h" in dot_contents
    assert "a_trace.c" in listing_contents and "b_trace.h" in listing_contents
