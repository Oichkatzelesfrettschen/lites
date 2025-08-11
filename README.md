# Lites

Lites is a 4.4BSD based Unix server that runs on top of the Mach
microkernel.  This repository collects several historical releases and
patch sets.

The tree is preserved mainly for reference.  Should a directory named
`lites-1.1-2025` (or similarly named) appear, it represents an ongoing
modernization effort and is not part of the original snapshots.  A
The modern tree consolidates historical releases and ongoing improvements.

Much of the original documentation and source code mirrors have long
vanished from the Internet.  The list below records a number of archival
links that are still reachable, mostly via the Internet Archive.

## Historical links

- http://web.archive.org/web/20031205031618/http://www.cs.hut.fi/~jvh/lites.html
- https://web.archive.org/web/19970716001117/http://www.cs.hut.fi/~jvh/lites-1.0-announcement.html
- http://ftp.funet.fi/pub/mach/lites/
- http://ftp.funet.fi/pub/mach/
- http://ftp.lip6.fr/pub/mach/mach4/mach/lites/
- http://ftp.lip6.fr/pub/mach/mach4/mach/
- https://www.cs.utah.edu/flux/lites/html/
- https://www.cs.utah.edu/flux/lites/html/lites-info.html
- https://www.cs.utah.edu/flux/oldsite.html
- https://www.cs.utah.edu/flux/oldproj.html
- https://www.cs.utah.edu/flux/mach4/html/Mach4-proj.html
- https://www.cs.utah.edu/flux/mach4-parisc/html/pamach.html
- http://www.cs.cmu.edu/afs/cs.cmu.edu/project/mach/public/www/mach.html
- http://web.archive.org/web/20020420174411/http://www-2.cs.cmu.edu/afs/cs.cmu.edu/project/art-6/www/rtmach.html
- http://web.archive.org/web/19990502223408/https://www.cs.utah.edu/projects/flux/mach4-i386/html/mach4-UK22.html
- http://web.archive.org/web/19990508181023/http://www.cs.utah.edu/projects/flux/
- http://web.archive.org/web/20031206081657/http://www-2.cs.cmu.edu/afs/cs/project/mach/public/www/sources/sources_top.html

For an overview of the available releases and a script that can generate diff patches between them, see [docs/HISTORY.md](docs/HISTORY.md).

## Directory layout

The repository now follows a simplified hierarchy.  Core kernel code lives in
`core/` while hardware drivers reside in `drivers/`.  Libraries shared by the
servers and tests live under `libs/` and the POSIX and capability aware
services themselves in `servers/`.  Small command line utilities have been
gathered in `util/` and demonstration programs live in `examples/`.  External
code that needs to remain in-tree is stored in `third_party/`.  Older release
snapshots and archived build files (such as `makefile.legacy`) are located in
the top level `archives/` directory.
Public headers now live in `include/` while kernel-only headers remain in
`core/include/`.


The file `johannes_helander-unix_under_mach-the_lites_server.pdf` in this
repository contains a comprehensive thesis describing Lites' design in
detail.

## Mach kernel headers

Lites relies on headers from a Mach kernel source tree such as the
OpenMach/Mach4 distribution.  The modern build system looks for these
headers in the directory specified by the `LITES_MACH_DIR` environment
variable.  If that variable is unset and a directory named `openmach`
exists at the repository root, it will be used automatically (a git
submodule can conveniently provide it).
Running `setup.sh` will automatically clone the OpenMach repository when
network access is available.  Pass `--offline` to skip the clone and
install any `.deb` archives found in `offline_packages/` with `dpkg -i`.
If prebuilt Mach libraries are present, set `LITES_MACH_LIB_DIR` to their
location (for example `openmach/lib`).  When the variable is not set and
`openmach/lib` exists, it will be used automatically.

When neither `LITES_MACH_DIR` nor an `openmach` directory are available, run
`scripts/import-mach-headers.sh` to populate `localmach/include` from either a
checked out OpenMach tree or the xkernel sources bundled with this repository.

Example using `Makefile.new`:

```sh
git submodule add <mach-repo-url> openmach  # once
make -f Makefile.new LITES_MACH_DIR=openmach \
    LITES_MACH_LIB_DIR=openmach/lib
```

Or with CMake:

```sh
cmake -B build -DLITES_MACH_DIR=openmach \
      -DLITES_MACH_LIB_DIR=openmach/lib
cmake --build build
```
## Header inventory

Use `scripts/flatten-headers.sh` to gather all header files from across the repository. The script copies each header into the `flattened_include/` directory and automatically renames duplicates by embedding the original path in the filename.

## Building

Lites requires a Mach 3 or Mach 4 kernel and a 4.4BSD userland.  Each
release is provided as a `*.tar.gz` archive.  Unpack the desired
version and build it inside the resulting directory.  For example:

```sh
tar -xzf lites-1.1.u3.tar.gz
cd lites-1.1.u3
make -f Makefile.new    # or run cmake -B build && cmake --build build
```

For the modernized build system in this repository you can also use
`Makefile.new` or the provided CMake files.  Both build from the directory
specified by `SRCDIR`/`LITES_SRC_DIR`.  Set this variable to point at the
legacy source tree.  The tools recognise an
`ARCH` variable which selects the target CPU.  Supported values include the
64‑bit `x86_64` and `riscv64`, 32‑bit `i686`, `arm` and `powerpc`, and the
16‑bit `ia16`.  The default is `ARCH=x86_64`.

Use `ARCH=ia16` for 16‑bit builds.  Choose `ARCH=i686`, `ARCH=arm` or
`ARCH=powerpc` for 32‑bit binaries, and `ARCH=x86_64` or `ARCH=riscv64` for
64‑bit targets.
Set `SANITIZE=1` to compile with address sanitizer support, which appends
`-fsanitize=address` to `CFLAGS` and `LDFLAGS`.
Examples:

```sh
# Using Makefile.new
make -f Makefile.new ARCH=i686
make -f Makefile.new ARCH=x86_64
make -f Makefile.new ARCH=arm
make -f Makefile.new ARCH=riscv64
make -f Makefile.new ARCH=powerpc
make -f Makefile.new ARCH=ia16
make -f Makefile.new ARCH=x86_64 SANITIZE=1

# Using CMake (optionally override the source directory)
cmake -B build -DARCH=i686
cmake -B build -DARCH=x86_64 -DLITES_SRC_DIR=/path/to/lites
cmake -B build -DARCH=arm
cmake -B build -DARCH=riscv64
cmake -B build -DARCH=powerpc
cmake -B build -DARCH=ia16
cmake --build build
```

To compile with Clang instead of GCC set the C compiler explicitly:

```sh
cmake -B build -DCMAKE_C_COMPILER=clang -DCMAKE_C_FLAGS=-O3
cmake --build build
```

### Building with Ninja

Use the Ninja generator for faster incremental builds:

```sh
cmake -G Ninja -B build -DARCH=x86_64
ninja -C build
```


### Optional dependencies

The root `CMakeLists.txt` detects certain tools only when available.  For
instance Bison is located via `find_package(BISON)`.  A typical snippet
looks like:

```cmake
find_package(BISON)
if(BISON_FOUND)
    BISON_TARGET(MyParser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.c)
    target_sources(lites_server PRIVATE ${BISON_MyParser_OUTPUTS})
endif()
```

The optional `setup.sh` script installs a wide range of cross-compilers
and emulators along with standard build utilities such as build-essential,
GCC, clang, llvm, m4, CMake and Ninja. BSD make (`bmake`) and the optional
`mk-configure` framework are installed as well. The script also sets up
debugging and profiling tools, installs the pre-commit hooks and generates a
`compile_commands.json` database for clang tooling. Run `pre-commit run --files
<path/to/file.c>` to format or analyse individual changes, or `pre-commit run -a`
to sweep the entire tree. The configuration invokes `clang-format` and
`clang-tidy` on tracked C and C++ sources while automatically skipping archival
directories such as `archives/`, `Historical Archives/`, `offline_packages/` and
`third_party/`. The script installs `pre-commit` via pip when missing and
ensures a `.pre-commit-config.yaml` file exists. It also verifies that `yacc`
(via `byacc` or `bison`) and the Swift toolchain are available, falling back to
additional package installs if necessary. Any package failures are recorded in
`/tmp/setup_failures.log` so the remainder of the setup can continue. The script
normally requires root privileges and network access. When invoked with
`--offline` it installs all `.deb` files from the `offline_packages/` directory
instead of using the network.
The package list now includes `tmux` for convenient session management. Cross-compilers for AArch64, ARMv7, PowerPC and RISC-V are installed so the tree can be built for a variety of targets. `gcc-multilib` conflicts with these cross toolchains, so it is omitted. `setup.sh` defaults to **clang-18**, switches to **clang-20** when available and falls back to **clang-14** or **clang-11** on older systems.
Wrapper symlinks named `cc` and `c++` point to the selected clang version so existing build scripts use clang transparently.

### 80386 optimisation flags

`setup.sh` exports a dense suite of options tuned for 80386 silicon. All
post‑486 instructions are disabled, loops and functions align to 16 bytes
and Polly/Graphite are enabled via `CLANG_EXTRA_FLAGS`. Unit tests inherit
these settings through the `CFLAGS` environment variable so the entire
project builds with `-march=i386 -mtune=i386` and the accompanying feature
restrictions. The key compiler options include:

```
-march=i386 -mtune=i386 -m32 \
-mno-sse -mno-sse2 -mno-sse3 -mno-ssse3 \
-mno-sse4 -mno-sse4.1 -mno-sse4.2 \
-mno-avx -mno-avx2 -mno-avx512f \
-mno-mmx -mno-3dnow \
-mno-cx8 -mno-cx16 -mno-sahf -mno-movbe \
-mno-aes -mno-sha -mno-pclmul -mno-popcnt -mno-abm \
-mno-lwp -mno-fma -mno-fma4 -mno-xop \
-mno-bmi -mno-bmi2 -mno-tbm -mno-lzcnt \
-mno-rtm -mno-hle -mno-rdrnd -mno-f16c \
-mno-fsgsbase -mno-rdseed -mno-prfchw -mno-adx \
-mno-fxsr -mno-xsave -mno-xsaveopt \
-fno-builtin-bswap16 -fno-builtin-bswap32 -fno-builtin-bswap64 \
-mfpmath=387 -m80387 -mhard-float -fexcess-precision=standard \
-ffp-contract=off \
-malign-data=compat -malign-functions=16 -malign-jumps=16 \
-malign-loops=16 -mstack-alignment=4 \
-mstringop-strategy=libcall \
-O3 -fomit-frame-pointer -fstrict-aliasing -fno-semantic-interposition \
-fmerge-all-constants -fno-common -ffunction-sections -fdata-sections \
-falign-functions=16 -falign-loops=16 \
-foptimize-sibling-calls -findirect-inlining -finline-limit=1000 \
-frename-registers -fweb -fira-algorithm=CB -fira-hoist-pressure \
-fsched-pressure -fsched-spec-load -fmodulo-sched \
-fmodulo-sched-allow-regmoves \
-fprofile-use=/tmp/pgo-386 -fauto-profile=/tmp/386-profile.afdo \
-mcmodel=small -mplt -mno-tls-direct-seg-refs
```

Codex CLI can keep `setup.sh` in sync with `.codex/setup.sh` automatically. The `postCreateCommand` in `.devcontainer/devcontainer.json` installs Codex and runs `codex -q 'doctor setup.sh'` after container creation. A sample systemd unit `scripts/codex-setup.service` demonstrates how to do the same early in boot.

### Codex environment packages

The [`setup.sh`](setup.sh) helper installs Clang, LLD and auxiliary tools, plus
debuggers, cross-compilers and QEMU.  The main packages include:

```
build-essential git wget curl
clang-<version> lld-<version> llvm-<version>-dev libclang-<version>-dev libpolly-<version>-dev
clang-tools-<version> clang-tidy-<version> clang-format-<version> clangd-<version>
ccache lldb gdb bolt llvm-bolt cmake make ninja-build meson
doxygen graphviz python3-sphinx python3-breathe shellcheck yamllint
python3 python3-pip python3-venv python3-setuptools python3-wheel
bison byacc nodejs npm yarnpkg coq coqide tla4tools isabelle
afl++ honggfuzz cargo-fuzz qemu-system-x86 qemu-utils valgrind lcov gcovr
tmux cloc cscope libperl-dev gdb-multiarch lizard
gcc-aarch64-linux-gnu gcc-arm-linux-gnueabihf gcc-riscv64-linux-gnu gcc-powerpc-linux-gnu
```

The script exports 80386-tuned optimisation flags and sets `cc`/`c++` to the
selected clang version. It also generates `compile_commands.json` so clang tools
work out of the box.


You can also invoke `scripts/run-precommit.sh` which automatically installs
`pre-commit` via pip when missing.

The clang-tidy hooks rely on `scripts/run-clang-tidy.sh`.  This helper
ensures a `compile_commands.json` database is generated on demand so
clang-tidy can analyse the sources even before the project has been built.

Alternatively CMake can drive clang-tidy directly.  Enable the optional
`ENABLE_CLANG_TIDY` flag to add a `clang-tidy` target:

```sh
cmake -DENABLE_CLANG_TIDY=ON -B build
cmake --build build --target clang-tidy
```

To try the built binaries under QEMU use `scripts/run-qemu.sh`.  The script
launches the `lites_server` (and the optional `lites_emulator` when present)
inside QEMU for either `x86_64` or `i686`:

```sh
# default to x86_64
scripts/run-qemu.sh

# explicitly run the 32‑bit build
scripts/run-qemu.sh i686
```
For a tmux-based setup use `scripts/tmux-qemu.sh`, which opens `run-qemu.sh` in one pane and a shell in another.

The helpers expect the binaries produced by `Makefile.new` in the repository
root and require the corresponding `qemu-system` binary in `PATH`.
For a split view with QEMU and an additional shell, run
`scripts/tmux-qemu.sh` instead of invoking `scripts/run-qemu.sh`
directly.

### tmux workflow

Run `scripts/tmux-qemu.sh` to open a tmux session with QEMU in the left pane
and an interactive shell on the right. The optional architecture argument is
forwarded directly to `scripts/run-qemu.sh`. See
[docs/tmux.md](docs/tmux.md) for more tips.

### QEMU troubleshooting

If QEMU fails due to an executable stack warning, link `lites_server` with
`-Wl,-z,noexecstack`.

Additional notes are kept in [docs/INDEX.md](docs/INDEX.md).  A detailed tmux example is provided in [docs/tmux.md](docs/tmux.md).  The mailbox-based IPC wrappers are described in [docs/IPC.md](docs/IPC.md).  Helper wrappers for common POSIX operations are documented in [docs/POSIX.md](docs/POSIX.md).
Design notes on the hybrid kernel approach and namespace algebra can be found in the remaining documentation.
## Tests

The historical unit tests depend on full Mach headers and are disabled in
this simplified repository snapshot.

## Header consolidation
A helper script `scripts/move_all_headers.sh` is provided to move all `*.h` files
from the repository into `$LITES_SRC_DIR/include/all_headers` while
preserving their original relative paths. This operation is destructive and
should only be run for analysis purposes.


## License map
The script `scripts/gen-license-map.sh` scans all source files and produces `LICENSE_MAP`.
Each line contains the path, detected license type, origin and last modification timestamp.
New files contain a short header referring to this map instead of duplicating the full license text.

## Code statistics

The repository contains a mix of historical sources and modernized code. The
table below summarises the approximate size of the tree using `cloc`:

```
Language                             files          blank        comment    code
HTML                                  1869         105058           6103   390749
C                                     1058          50906         105064   336219
XML                                    805              6              0   286249
C/C++ Header                           980          13120          42509    60471
C++                                     29           4033           7092    24629
Assembly                               138           3533           7707    18269
Bourne Shell                            35           1332           1585     8320
TeX                                     45           1705           1421     5401
```

Overall the tree spans over **1,142,516** lines of code across more than five
thousand files.

## Memory allocation roadmap

The current DDEKit memory API exposes `ddekit_contig_malloc()` for obtaining
DMA-safe contiguous buffers. Drivers that merely require virtually contiguous
regions must still rely on this low-level call. A vmalloc-style allocator is
planned for future versions to handle non-contiguous allocations gracefully.
