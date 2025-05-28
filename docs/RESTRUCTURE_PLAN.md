# Repository Restructure Plan

This document outlines the proposed reorganisation of the source tree.  The goal
is to clarify the roles of each component and make the build system easier to
maintain.

## Goals

- Separate kernel, user space servers and libraries into clear top level
  directories.
- Move historical snapshots and old build scripts (e.g. `makefile.legacy`)
  under `Historical Archives/`.
- Provide a common `third_party/` directory for external projects.
- Consolidate build scripts under a single `cmake/` hierarchy.
- Collect small utilities in `util/` and place example programs in
  `examples/`.

## Phases

1. **Layout preparation** – introduce the new directory structure and update
   include paths.
2. **Source migration** – move existing files into their new locations while
   keeping history intact.
3. **Build updates** – adjust CMake and Makefile logic to match the layout.
4. **Clean up** – drop obsolete scripts and verify all tests still build.
5. **Post-move build** – perform a clean build and run the unit tests to ensure
   the new paths work correctly.

Further discussion and detailed tasks will be tracked in issue notes.

