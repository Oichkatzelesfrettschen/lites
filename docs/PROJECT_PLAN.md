# Project Plan

This file collects small tasks and utilities used during the
ongoing reorganisation work.

## Scripts

- **scripts/reorg/flatten_tree.sh** – copies sources from the
  `exokernel/`, `microkernel/` and `libOS/` directories into
  `flattened/` with path components encoded using underscores.
- **scripts/reorg/include_graph.sh** – parses `#include` lines in the
  tree and writes a simple `source -> header` map to
  `include_graph.txt` by default.
