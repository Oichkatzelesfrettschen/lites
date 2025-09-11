#!/usr/bin/env python3
"""
@file trace_graph.py
@brief Construct a GraphViz DOT file capturing include relationships
       among source files containing the string ``trace`` either in the
       filename or file contents.

The script scans the repository, detects files that either mention
``trace`` in their name or contain the word inside their body and then
builds a directed graph showing which of those files include each other.
The resulting graph is written to ``trace_graph.dot`` in the current
working directory.  Nodes are full file paths and edges represent
``#include`` directives referencing another trace-related file.

This tool is primarily intended to help developers reason about the
interconnections between trace facilities across the code base.
"""
from __future__ import annotations

import argparse
import re
from pathlib import Path

try:
    import networkx as nx
    import pydot  # type: ignore[import-not-found]  # Imported for its GraphViz bindings
except ModuleNotFoundError as exc:
    raise SystemExit(
        "networkx and pydot are required to generate trace graphs; install with 'pip install networkx pydot'."
    ) from exc


def find_trace_files(root: Path, exclude: set[str] | None = None) -> set[Path]:
    """Locate files mentioning ``trace`` either in their name or contents.

    @param root     Root directory to scan.
    @param exclude  Set of directory names to omit from the search.
    @return         Set of absolute ``Path`` objects to matching files.
    """

    trace_files: set[Path] = set()
    excluded = exclude or set()
    for path in root.rglob("*"):
        if any(part in excluded for part in path.parts):
            continue
        if not path.is_file():
            continue
        if "trace" in path.name:
            trace_files.add(path)
            continue
        try:
            content = path.read_text(errors="ignore")
        except Exception:
            continue
        if "trace" in content:
            trace_files.add(path)
    return trace_files


def build_graph(trace_files: set[Path], root: Path) -> nx.DiGraph:
    """Create include graph for trace files.

    @param trace_files  Set of files that contain trace data.
    @param root         Repository root used to resolve relative includes.
    @return             Directed graph of include relationships.
    """
    graph = nx.DiGraph()
    include_re = re.compile(r"#include\s+[<\"]([^>\"]+)[>\"]")
    normalized = {str(path): path for path in trace_files}
    for src in trace_files:
        src_key = str(src)
        graph.add_node(src_key)
        try:
            text = src.read_text(errors="ignore")
        except Exception:
            continue
        for match in include_re.findall(text):
            if "trace" not in match:
                continue
            candidate = (root / match).resolve()
            dst_key = str(normalized.get(str(candidate), candidate))
            graph.add_edge(src_key, dst_key)
    return graph


def main() -> None:
    """Program entry point.

    @return Nothing.
    """

    parser = argparse.ArgumentParser(
        description=(
            "Construct a GraphViz DOT graph describing include relationships among "
            "files that reference the token 'trace'."
        )
    )
    parser.add_argument(
        "-r",
        "--root",
        type=Path,
        default=Path(__file__).resolve().parents[1],
        help="Root directory to scan. Defaults to repository root.",
    )
    parser.add_argument(
        "-o",
        "--output",
        type=Path,
        default=Path("trace_graph.dot"),
        help="Destination DOT file. Defaults to 'trace_graph.dot' in current directory.",
    )
    parser.add_argument(
        "--files",
        type=Path,
        default=None,
        help="Optional path to write a newline-separated list of trace files.",
    )
    parser.add_argument(
        "--exclude",
        action="append",
        default=[".git"],
        help="Directory name to exclude; may be provided multiple times.",
    )
    args = parser.parse_args()

    repo_root = args.root.resolve()
    trace_files = sorted(find_trace_files(repo_root, set(args.exclude)))
    graph = build_graph(set(trace_files), repo_root)
    nx.drawing.nx_pydot.write_dot(graph, args.output.resolve())
    if args.files is not None:
        args.files.write_text("\n".join(str(p) for p in trace_files) + "\n")


if __name__ == "__main__":
    main()
