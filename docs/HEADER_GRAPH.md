# Header Dependency Graph

This document records relationships between headers in the `include/`
directory. The `scripts/header_graph.py` utility parses `#include` lines and
produces a simple directed graph of the dependencies.

Run the script from the repository root.  To analyse only the `include/`
directory use:

```sh
python3 scripts/header_graph.py --root include --dot docs/header_graph.dot
```

For a repository-wide inventory of all headers, pass the repository root as the
`--root` argument and generate an alternate graph file:

```sh
python3 scripts/header_graph.py --root . --dot docs/header_graph_full.dot
```

The script prints any detected include cycles and writes a Graphviz `DOT`
file to the path specified with `--dot` (for example
`docs/header_graph.dot` or `docs/header_graph_full.dot`). Render the file
with `dot -Tpng` or a compatible tool to visualise the graph.

## Detected cycles

```
sys/ushared.h -> sys/proc.h -> sys/shared_lock.h
```

This cycle indicates tight coupling between the `sys` headers. Breaking it
could reduce build dependencies.
