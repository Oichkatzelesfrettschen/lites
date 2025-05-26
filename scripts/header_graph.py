#!/usr/bin/env python3
"""Generate a header include graph and detect cycles."""
import os
import re
import argparse

INCLUDE_RE = re.compile(r"^\s*#\s*include\s*[\"<](.*?)[\">]")


def parse_headers(root: str):
    graph = {}
    for dirpath, _, files in os.walk(root):
        for name in files:
            if not name.endswith(".h"):
                continue
            path = os.path.join(dirpath, name)
            if not os.path.isfile(path):
                # skip dangling symlinks or other non-regular files
                continue
            key = os.path.relpath(path, root)
            graph.setdefault(key, [])
            with open(path, "r", errors="ignore") as f:
                for line in f:
                    m = INCLUDE_RE.search(line)
                    if not m:
                        continue
                    inc = m.group(1)
                    target = os.path.join(root, inc)
                    if os.path.exists(target):
                        graph[key].append(os.path.relpath(target, root))
    return graph


def write_dot(graph, out_path: str):
    with open(out_path, "w") as f:
        f.write("digraph includes {\n")
        for src, tgts in graph.items():
            for tgt in tgts:
                f.write(f'    "{src}" -> "{tgt}";\n')
        f.write("}\n")


def tarjan(graph):
    index = 0
    stack = []
    on_stack = set()
    indexes = {}
    lowlinks = {}
    components = []

    def strongconnect(v):
        nonlocal index
        indexes[v] = index
        lowlinks[v] = index
        index += 1
        stack.append(v)
        on_stack.add(v)
        for w in graph.get(v, []):
            if w not in indexes:
                strongconnect(w)
                lowlinks[v] = min(lowlinks[v], lowlinks[w])
            elif w in on_stack:
                lowlinks[v] = min(lowlinks[v], indexes[w])
        if lowlinks[v] == indexes[v]:
            comp = []
            while True:
                w = stack.pop()
                on_stack.remove(w)
                comp.append(w)
                if w == v:
                    break
            if len(comp) > 1:
                components.append(comp)

    for v in graph:
        if v not in indexes:
            strongconnect(v)
    return components


def main():
    parser = argparse.ArgumentParser(description="Analyse header includes")
    parser.add_argument(
        "--root", default="include", help="Directory containing headers"
    )
    parser.add_argument(
        "--dot", default="docs/header_graph.dot", help="Output DOT file"
    )
    args = parser.parse_args()
    graph = parse_headers(args.root)
    write_dot(graph, args.dot)
    cycles = tarjan(graph)
    if cycles:
        print("Cycles detected:")
        for c in cycles:
            print(" -> ".join(c))
    else:
        print("No cycles detected")


if __name__ == "__main__":
    main()
