# Project Plan

This document summarises the major phases of the modernisation effort. Each section outlines core objectives and references related documentation. The work items are loosely coupled so teams can progress in parallel with minimal interference.

## Archaeological Analysis and Inventory
- Catalog existing source drops and historical patches.
- Record unique files and configuration fragments to preserve during cleanup.

## Functional Decomposition
- Identify discrete subsystems in the original code base.
- Map each subsystem to its Mach or BSD counterpart for easier replacement.

## Dependency Graph Construction
- Build a high-level graph showing which subsystems rely on others.
- Use the graph to prioritise independent tasks and uncover hidden ties.

## Strategic Flattening
- Consolidate duplicated headers and utilities across releases.
- Prepare a single directory structure that new components can target.

## Header Dependency Resolution
- Remove obsolete include guards and outdated macros.
- Ensure headers expose only the interfaces required by dependent modules.

## Architectural Modernization Strategy
- Plan incremental updates to C23 and modern toolchains.
- Coordinate with the ongoing tasks described in [MODERNIZATION.md](MODERNIZATION.md) and [INTEGRATION_PLAN.md](INTEGRATION_PLAN.md).

## Multiple Userspace Support
- Define how various user-mode servers or libraries coexist on top of Mach.
- Isolate shared resources so alternative runtime environments do not conflict.

## Build System Modernization
- Transition legacy makefiles to the new build infrastructure.
- Keep compatibility layers thin so developers can experiment with CMake or Meson.

## Testing and Validation Strategy
- Establish automated tests covering POSIX layers and kernel interfaces.
- Integrate static checks and runtime tests to validate modernised modules.

