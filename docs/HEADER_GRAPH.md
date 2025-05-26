# Header Dependency Graph

This document records relationships between headers in the `include/`
directory. The `scripts/header_graph.py` utility parses `#include` lines and
produces a simple directed graph of the dependencies.

Run the script from the repository root:

```sh
python3 scripts/header_graph.py --root include --dot docs/header_graph.dot
```

The script prints any detected include cycles and writes a Graphviz `DOT`
file to `docs/header_graph.dot`. Render the file with `dot -Tpng` or a
compatible tool to visualise the graph.

## Detected cycles

```
sys/ushared.h -> sys/proc.h -> sys/shared_lock.h
```

This cycle indicates tight coupling between the `sys` headers. Breaking it
could reduce build dependencies.
