# Modernization Status

The modernised tree (in `$LITES_SRC_DIR`) contains ongoing work to bring the
original Lites sources forward.  This document summarises what has
already been completed and what is still planned.

## Completed tasks

- Migration of the code base to the C23 language standard.
- New build system using `Makefile.new` and CMake with automatic
  detection of Mach headers and optional libraries.
- Support for 64‑bit (`ARCH=x86_64`) and 32‑bit (`ARCH=i686`) builds.
- Pre‑commit configuration for consistent formatting and clang‑tidy
  checking.
- A generated `LICENSE_MAP` summarises the license excerpt, origin and
  last modification date for every source file via `scripts/gen-license-map.sh`.

## Open items

- Re‑enable additional architectures (ARM, PowerPC, RISC‑V, etc.).
- Modernise the emulator support and verify cross‑builds.
- Expand documentation for cross‑compiling and runtime testing.
- Audit remaining TODO comments in the source tree.
- Convert header prototypes still guarded by `__STDC__` or using
  the legacy `__P()` macro. Examples include
  `xkernel/include/event.h`, `xkernel/include/msg.h` and numerous
  Mach headers.
- Replace use of `bzero`, `bcopy` and `bcmp` with their modern
  `memset`, `memcpy` and `memcmp` equivalents. More than eighty
  occurrences remain across files such as
  `protocols/join/join.c`, `protocols/chan/chan.c`,
  `protocols/udp/udp.c` and several Mach drivers.
- Remove `register` qualifiers and update old style function
  declarations.  `protocols/tcp-tahoe/tcp_output.c`,
  `protocols/tcp-tahoe/tcp_subr.c` and `protocols/pmap/xkpm.c`
  still require modern prototypes.

### Mach-specific TODOs

- `mach3/drivers/xec596/xec596.c` – TODO: "Go native" for the Intel 82596 driver implementation.
- `mach4/drivers/xec596/xec596.c` – same TODO as above.
- `mach4/pxk/time.c` uses placeholder values for
  `xGetTime()` marked as "FAKE! TODO! PNR" and needs a proper Mach
  time source.
- A helper script `import-mach-headers.sh` now copies Mach headers from either
  an OpenMach tree or the bundled xkernel sources into `localmach/include` for
  systems without an external Mach source.

