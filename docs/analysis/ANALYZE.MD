# Analysis Log

This document records the tooling setup and code analysis steps performed for trace connectivity exploration.

## Environment Preparation
The following tooling suite was installed to enable multi-angle trace analysis.

### System Packages
Installed via `apt` with verified versions:

- `ripgrep` **14.1.0** – fast recursive searcher
- `universal-ctags` **5.9.0** – tag generation for cross‑referencing
- `cscope` **15.9** and `global` **6.6.11** – code navigation databases
- `cloc` **1.98** – line-of-code metrics
- `graphviz` **2.43.0** – rendering of DOT graphs
- `bpftrace` **0.20.2** and `bpfcc-tools` **0.29.1** – eBPF tracing utilities
- `doxygen` **1.9.8** – documentation extractor
- `diffoscope` **259** – deep diff inspection

### Python Packages
Installed via `pip`:

- `networkx` **3.5** – graph manipulation
- `pydot` **4.0.1** – DOT export
- `lizard` **1.17.31** – complexity metrics
- `jupyterlab` **4.4.5** – interactive exploration

### Node Package
Installed globally via `npm`:

- `tree-sitter-cli` **0.25.8** – AST parsing

### Additional Tools to Explore
- `Sphinx` for documentation
- `perf`, `SystemTap`, or `ftrace` for complementary runtime tracing
- `CodeQL` or `Joern` for code property graphs
- `DTrace` where supported to compare probe coverage

## Trace File Enumeration
Executed the include-graph generator to locate trace-related files while excluding third-party sources:
```bash
python scripts/trace_graph.py --files trace_files.txt --exclude third_party
```
This produced **543** trace-associated files. A sample of the discovered paths:
```
/workspace/lites/Items1/lites-1.0/include/alpha/ptrace.h
/workspace/lites/Items1/lites-1.0/include/i386/ptrace.h
/workspace/lites/servers/sunos/pxk/trace.c
/workspace/lites/util/compose/compose.c
```
The complete list resides in `trace_files.txt` (not committed).

## Graph Generation
A DOT graph describing inter-file `#include` relationships was emitted via:
```bash
python scripts/trace_graph.py -o trace_graph.dot --exclude third_party
```
The resulting `trace_graph.dot` can be rendered with Graphviz:
```bash
dot -Tpng trace_graph.dot -o trace_graph.png
```

These steps provide a reproducible baseline for further static or dynamic trace investigations.
## Additional Tooling
The following packages were installed to facilitate building and documentation:

- `bmake` – BSD make implementation enabling legacy makefiles.
- `doxygen` – documentation generator for C/C++ sources.
