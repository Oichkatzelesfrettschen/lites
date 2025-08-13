# Building Lites

This guide details a reproducible setup for compiling the historical sources
included in this repository.  The steps install all development tools,
configure the tree and attempt a build with full instrumentation.

## 1. Environment setup

Follow the [environment setup guide](setup.md) to install compilers,
debuggers and tracing utilities. The steps require root privileges when
adding packages.

The guide installs clang, lld, qemu, valgrind, strace, ltrace, perf and
assorted cross compilers. Use `scripts/check_build_tools.sh` to verify that
all core tools were found.

## 2. Configure the source tree

Generate a build tree with CMake using the Ninja generator and the LLVM
toolchain. The legacy sources live under `Items1/lites-1.1.u3`:

```bash
cmake -G Ninja -B build \
      -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ \
      -DCMAKE_LINKER=lld \
      -DLITES_SRC_DIR=Items1/lites-1.1.u3
ninja -C build
```

The first build may fail because Mach headers are missing. Populate them
using `scripts/extract-xmach-headers.sh` and rerun `ninja`:

```bash
LITES_SRC_DIR=Items1/lites-1.1.u3 scripts/extract-xmach-headers.sh
ninja -C build
```

## 3. Instrumentation

Once compilation succeeds you can run the server under QEMU with extra
instrumentation.  The following example executes the image with valgrind and
records a trace:

```bash
scripts/tmux-qemu.sh &
valgrind --track-origins=yes --log-file=valgrind.log \
    qemu-system-x86_64 -kernel build/lites
```

## 4. Troubleshooting

Missing headers or packages will be reported in the logs mentioned above.
Ensure `LITES_MACH_DIR` points at a compatible Mach kernel tree if local
headers are insufficient.

## 5. Pre-commit hooks

Install the local hooks after cloning the repository and run them
whenever sources are modified.  They enforce formatting and basic lint
rules using `clang-format`, `clang-tidy` and standard whitespace checks.

```bash
pre-commit install        # set up git hooks
pre-commit run -a         # format and lint the entire tree
```

These hooks rely on the configuration in `.pre-commit-config.yaml` and
operate automatically during commits.
