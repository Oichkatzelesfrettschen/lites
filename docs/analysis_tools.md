# Analysis and Instrumentation Tooling

A reliable software analysis stack starts with installations sourced from authoritatively documented origins. Package managers and registries such as the Debian archive, PyPI and npm distill community consensus on trusted distribution paths, while some projects continue to prefer source builds from version‑controlled repositories. The matrix below catalogues common utilities for auditing and instrumenting this repository, recording how each tool is installed, the configuration or invocation used here, and where its output resides under `docs/tool_reports/`.

```
+-------------------+---------------------------------------+-----------------------------------------+--------------------------------------------+
| Tool              | Install Command                       | Configuration / Invocation               | Report                                     |
+-------------------+---------------------------------------+-----------------------------------------+--------------------------------------------+
| lizard            | pip install lizard                    | `lizard core`                           | `docs/tool_reports/lizard_summary.md`      |
| cloc              | sudo apt install cloc                 | `cloc .`                                | `docs/tool_reports/cloc.md`                |
| cscope            | sudo apt install cscope               | `cscope -R -b`                          | `docs/tool_reports/cscope.md`              |
| diffoscope        | pip install diffoscope                | `diffoscope README.md README.md`        | `docs/tool_reports/diffoscope.md`          |
| dtrace            | git clone https://github.com/dtrace4linux/linux.git; cd linux; make; sudo make install | not executed                               | _not run_                                  |
| valgrind          | sudo apt install valgrind             | `valgrind --version`                    | `docs/tool_reports/valgrind.md`            |
| cppcheck          | sudo apt install cppcheck             | `cppcheck --enable=warning,style core/*.c` | `docs/tool_reports/cppcheck.md`        |
| sloccount         | sudo apt install sloccount            | `sloccount core`                        | `docs/tool_reports/sloccount_summary.md`   |
| flawfinder        | sudo apt install flawfinder           | `flawfinder core`                       | `docs/tool_reports/flawfinder.md`          |
| gdb               | sudo apt install gdb                  | `gdb --version`                         | `docs/tool_reports/gdb.md`                 |
| pylint            | pip install pylint                    | `pylint docs/sphinx/conf.py scripts/trace_graph.py tests/test_trace_graph.py` | `docs/tool_reports/pylint.md` |
| flake8            | pip install flake8                    | `flake8 docs/sphinx/conf.py scripts/trace_graph.py tests/test_trace_graph.py` | `docs/tool_reports/flake8.md` |
| mypy              | pip install mypy                      | `mypy docs/sphinx/conf.py scripts/trace_graph.py tests/test_trace_graph.py`   | `docs/tool_reports/mypy.md`   |
| semgrep           | pip install semgrep                   | `semgrep --config auto core`            | `docs/tool_reports/semgrep.md`             |
| eslint            | npm install -g eslint                 | `eslint .`                              | `docs/tool_reports/eslint.md`              |
| jshint            | npm install -g jshint                 | `jshint .`                              | `docs/tool_reports/jshint.md`              |
| jscpd             | npm install -g jscpd                  | `jscpd core`                            | `docs/tool_reports/jscpd.md`               |
| nyc               | npm install -g nyc                    | `nyc --version`                         | `docs/tool_reports/nyc.md`                 |
+-------------------+---------------------------------------+-----------------------------------------+--------------------------------------------+
```

Many tools support alternate delivery channels—`cloc` can be fetched via `npm` and `diffoscope` is packaged for Debian—yet the commands above reflect the pathways endorsed by each project's primary documentation.
