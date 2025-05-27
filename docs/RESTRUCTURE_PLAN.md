# Repository Restructure Plan

This document outlines the proposed reorganisation of the source tree.  The goal
is to clarify the roles of each component and make the build system easier to
maintain.

## Goals

- Separate kernel, user space servers and libraries into clear top level
  directories.
- Move historical snapshots under `Historical Archives/`.
- Provide a common `third_party/` directory for external projects.
- Consolidate build scripts under a single `cmake/` hierarchy.

## Phases

1. **Layout preparation** – introduce the new directory structure and update
   include paths.
2. **Source migration** – move existing files into their new locations while
   keeping history intact.
3. **Build updates** – adjust CMake and Makefile logic to match the layout.
4. **Clean up** – drop obsolete scripts and verify all tests still build.

Further discussion and detailed tasks will be tracked in issue notes.

