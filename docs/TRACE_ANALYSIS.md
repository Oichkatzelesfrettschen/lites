# Trace Connectivity Analysis

`trace_graph.py` builds a directed include graph connecting source files that
reference the token `trace`. The graph highlights how tracing facilities are
interwoven across the project.

## Prerequisites

- Python 3.12+
- System packages: `ripgrep`, `universal-ctags`, `cscope`, `global`, `cloc`,
  `graphviz`, `bpftrace`, `bpfcc-tools`, `doxygen`, `diffoscope`
- Python packages: `networkx`, `pydot`, `lizard`, `jupyterlab`
- Node package: `tree-sitter-cli`

Install the tooling with:

```bash
sudo apt-get install ripgrep universal-ctags cscope global cloc graphviz \
  bpftrace bpfcc-tools doxygen diffoscope
pip install networkx pydot lizard jupyterlab
npm install -g tree-sitter-cli
```

## Usage

Run the utility to scan the repository and emit a DOT graph:

```bash
python scripts/trace_graph.py
```

Optional arguments allow customising the scan root, output location, writing the
list of discovered trace files, and excluding specific directories:

```bash
python scripts/trace_graph.py -r path/to/src -o my_graph.dot --files trace_files.txt --exclude third_party --exclude build
```

Render the resulting file with Graphviz:

```bash
dot -Tpng my_graph.dot -o trace_graph.png
```

## Implementation Overview

The utility performs two phases:

1. **Discovery** – Scans the tree for files that either contain `trace` in
   their name or within their contents.
2. **Graph Construction** – Parses each discovered file for `#include`
   directives referencing other trace-related files and builds a directed
   graph of these relationships.

The output provides a high-level view of trace-related interdependencies,
facilitating further dynamic instrumentation work.  For deeper studies, the
following staged workflow mirrors the broader trace-analysis methodology:

1. **Static extraction** – `rg`, `ctags`, and `tree-sitter` catalog trace
   symbols and their boundaries.
2. **Metrics** – `cloc` and `lizard` quantify trace-heavy regions.
3. **Dynamic validation** – runtime tools such as DTrace, `bpftrace`, `perf`,
   `SystemTap`, or `ftrace` confirm which probes are exercised.
4. **Visualisation** – `networkx` and Graphviz render inter-file and
   inter-probe relationships.
5. **Documentation** – Doxygen or Sphinx align code comments with the
   extracted structures.

This pipeline scales from static code inspection to live system tracing,
building a recursive map of trace interactions.
