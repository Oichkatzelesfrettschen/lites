# Environment Setup Guide

This guide replaces the previous `setup.sh` script and outlines manual steps to provision a development environment for building Lites.

## 1. Update the System

```bash
sudo apt update
sudo apt dist-upgrade -y
```

For environments without network access, download the required `.deb` archives on another machine and copy them into `offline_packages/`.

## 2. Clone OpenMach (Optional)

```bash
git clone https://github.com/machkernel/openmach.git openmach
```

If an `openmach` directory is present, the build system will automatically use it. Otherwise, populate `localmach/include` with `scripts/import-mach-headers.sh`.

## 3. Install Toolchain and Utilities

Install the packages that were previously pulled in by `setup.sh`:

```bash
sudo apt install -y \
  build-essential git wget curl \
  clang-18 lld-18 llvm-18-dev libclang-18-dev polly \
  clang-tools-18 clang-tidy-18 clang-format-18 clangd-18 \
  ccache lldb gdb bolt llvm-bolt \
  cmake make ninja-build meson \
  doxygen graphviz python3-sphinx python3-breathe \
  shellcheck yamllint \
  python3 python3-pip python3-venv python3-setuptools python3-wheel \
  bison byacc \
  nodejs npm yarnpkg \
  coq coqide tla4tools isabelle \
  afl++ honggfuzz cargo-fuzz \
  qemu-system-x86 qemu-utils valgrind lcov gcovr \
  tmux cloc cscope libperl-dev gdb-multiarch lizard \
  gcc-aarch64-linux-gnu gcc-arm-linux-gnueabihf \
  gcc-riscv64-linux-gnu gcc-powerpc-linux-gnu
```

### Tool overview

The packages above provide the full toolchain and analysis stack:

- **Clang/LLVM (clang-18, llvm-18, lld-18):** modern C/C++ compilation and
  linking using the LLD linker.
- **Ninja:** high‑speed build backend driven by CMake.
- **GDB and LLDB:** complementary debuggers for native and cross builds.
- **Valgrind and lcov:** dynamic analysis, memory checking and coverage
  reporting.
- **QEMU:** hardware emulation for running the server images.
- **Strace, ltrace and perf:** system call and performance tracing
  utilities.
- **Cross compilers (gcc-*-linux-gnu):** optional toolchains for
  architecture experimentation.

Additional auditing and instrumentation utilities, along with their recommended installation methods, are catalogued in [analysis_tools.md](analysis_tools.md).

## 4. Pre-commit Hooks

Ensure `pre-commit` is available and install the repository hooks:

```bash
pip3 install --user pre-commit
pre-commit install
```

Run `pre-commit run -a` to format and analyse the tree.

## 5. Generate Compile Commands

Create a compilation database to aid IDE integration:

```bash
cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

## 6. Environment Variables

The following exports mirror the defaults from the original script:

```bash
export CC="ccache clang-18"
export CXX="ccache clang++-18"
export CLANG_TIDY="clang-tidy-18"
export PATH="/usr/lib/ccache:$PATH"
export CFLAGS="-Wall -Wextra -Werror"
export CXXFLAGS="$CFLAGS"
export LDFLAGS="-fuse-ld=lld -flto -Wl,-z,noexecstack"
export LLVM_PROFILE_FILE="/tmp/profiles/default.profraw"
export CLANG_EXTRA_FLAGS="-mllvm -polly -mllvm -polly-opt-max-coefficient=32 -mllvm -polly-pattern-matching-based-opts=true -fgraphite-identity -fgraphite"
```

Adjust versions and optimisation flags as needed. The build system will now utilise these settings.

## 7. Completion

After completing the steps above the environment is ready to build the historical Lites sources using the instructions in `docs/build_steps.md`.

