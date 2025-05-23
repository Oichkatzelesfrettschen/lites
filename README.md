# Lites

Lites is a 4.4BSD based Unix server that runs on top of the Mach
microkernel.  This repository collects several historical releases and
patch sets.

The tree is preserved mainly for reference.  Should a directory named
`lites-1.1-2025` (or similarly named) appear, it represents an ongoing
modernization effort and is not part of the original snapshots.

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
network access is available.
If prebuilt Mach libraries are present, set `LITES_MACH_LIB_DIR` to their
location (for example `openmach/lib`).  When the variable is not set and
`openmach/lib` exists, it will be used automatically.

Example using the makefile:

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

## Building

Lites requires a Mach 3 or Mach 4 kernel and a 4.4BSD userland.  Each
release is provided as a `*.tar.gz` archive.  Unpack the desired
version and build it inside the resulting directory.  For example:

```sh
tar -xzf lites-1.1.u3.tar.gz
cd lites-1.1.u3
./conf/configure    # or use the ./configure symlink
make
```

For the modernized build system in this repository you can also use
`Makefile.new` or the provided CMake files.  Both automatically build from
the `src-lites-1.1-2025` directory when it is present.  Set the `SRCDIR` or
`LITES_SRC_DIR` variables to override this behaviour.  The tools recognise an
optional `ARCH` variable, selecting between 64‑bit (`ARCH=x86_64`, default) and
32‑bit (`ARCH=i686`) builds.  Other architectures were supported in historical
releases but are not handled by the modern build system.
Examples:

```sh
# Using the makefile
make -f Makefile.new ARCH=i686
make -f Makefile.new ARCH=x86_64

# Using CMake (optionally override the source directory)
cmake -B build -DARCH=i686
cmake -B build -DARCH=x86_64 -DLITES_SRC_DIR=src-lites-1.1-2025
cmake --build build
```

The optional `setup.sh` script installs a wide range of cross-compilers
and emulators along with standard build utilities such as build-essential,
GCC, clang, llvm, m4, CMake, Ninja and Meson.  BSD make (`bmake`) and the optional `mk-configure` framework are installed as well.  The script also sets up debugging and profiling tools, installs the pre-commit hooks and generates a
`compile_commands.json` database for clang tooling.  Run `pre-commit run -a`
after editing sources to keep formatting consistent.  The script installs `pre-commit` via pip when missing and ensures a `.pre-commit-config.yaml` file exists.  It also verifies
that `yacc` (via `byacc` or `bison`) and the Swift toolchain
are available, falling back to additional package installs if necessary.
Any package failures are recorded in `/tmp/setup_failures.log`
so the remainder of the setup can continue.  The script requires root
privileges and network access.

You can also invoke `scripts/run-precommit.sh` which automatically installs
`pre-commit` via pip when missing.

The clang-tidy hooks rely on `scripts/run-clang-tidy.sh`.  This helper
ensures a `compile_commands.json` database is generated on demand so
clang-tidy can analyse the sources even before the project has been built.

Additional notes are kept in [`docs/`](docs/).

