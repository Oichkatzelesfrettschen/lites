# Modernization Status

The `src-lites-1.1-2025` tree contains ongoing work to bring the
original Lites sources forward.  This document summarises what has
already been completed and what is still planned.

## Completed tasks

- Migration of the code base to the C23 language standard.
- New build system using `Makefile.new` and CMake with automatic
  detection of Mach headers and optional libraries.
- Support for 64‑bit (`ARCH=x86_64`) and 32‑bit (`ARCH=i686`) builds.
- Pre‑commit configuration for consistent formatting and clang‑tidy
  checking.

## Open items

- Re‑enable additional architectures (ARM, PowerPC, RISC‑V, etc.).
- Modernise the emulator support and verify cross‑builds.
- Expand documentation for cross‑compiling and runtime testing.
- Audit remaining TODO comments in the source tree.
- Provide a helper script to extract Mach headers from the xkernel
  sources into `localmach/include` for systems without an external
  Mach tree.
