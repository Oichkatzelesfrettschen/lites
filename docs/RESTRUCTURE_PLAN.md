# Source Tree Restructuring Plan

This document outlines the proposed layout changes for the modernised Lites
repository. The goal is a simpler hierarchy that separates the core kernel
code, hardware drivers and reusable libraries.

## Goals

- **`core/`** – contains the main kernel and POSIX compatibility sources.
- **`drivers/`** – holds platform specific or virtual hardware drivers.
- **`libs/`** – optional support libraries built alongside the kernel.

Placing related files under these top level directories should make navigation
and build rules easier to understand.

## Header location

Most headers will move next to their corresponding source files. Common
interfaces shared across multiple components remain under `include/common/`.
This keeps frequently used headers in a single place while still allowing
module specific headers to live with their sources.

## Build system updates

Both `CMakeLists.txt` and `Makefile.new` must be updated to search the new
directories. Include paths should be adjusted so headers in `include/common/`
are found automatically and each module adds its own directory. Targets that
previously referenced old paths need to be migrated to the new layout.

## Testing the restructure

1. Configure and build via CMake:
   ```
   cmake -B build
   cmake --build build
   ```
2. Build using the makefile:
   ```
   make -f Makefile.new
   ```
3. Run the existing test suite from the `tests/` directory to ensure
   functionality is unchanged.

