# POSIX Compliance Implementation Roadmap

This document consolidates the upstream modernization notes from the
repository with the proposed POSIX capability‑oriented design.  It
outlines staged work to reach a usable POSIX layer on top of the Lites
server.  Each phase summarises existing functionality and identifies
missing pieces.
The expanded vision unifies POSIX compatibility with a capability-based security model.  Core components will gradually gain machine-checkable specifications so that namespace operations and IPC paths can be verified with tools such as Coq or Lean.


## Phase I – Build and Core Infrastructure

* **Unified build system** – The repository already offers
  `Makefile.new` and CMake support for the modernised source tree.
  Additional tooling such as Meson is planned but not yet present.
  Cross‑compilation helpers live in `setup.sh`.
* **Header consolidation** – The `scripts/flatten-headers.sh` script gathers headers from the historical sources.
  Modernisation tasks include re‑enabling architectures beyond x86 and cleaning up obsolete prototypes.
* **Documentation pipeline** – Existing documentation resides under
  `docs/` with topics such as [`MODERNIZATION.md`](MODERNIZATION.md)
  and [`POSIX.md`](POSIX.md).
  A future step is generating reference material automatically.
* **Formal specification** – Core capability operations and namespace algebra will be described using proof assistants to guarantee compatibility and security.

## Phase II – Memory Management

The repository contains placeholder VM handlers
(`server/vm/vm_handlers.c`) which currently map anonymous pages when a
fault occurs.  Tests such as `tests/vm_fault` exercise this code.
Planned work includes:

* Implement page protection tracking and capability‑checked mappings.
* Introduce per‑process accounting structures and integrate them with
the pager interface used by the tests.
* Wrap POSIX memory calls (`mmap`, `mprotect`, `msync`) with capability
  enforcement similar to the IPC wrappers.

## Phase III – Process Management and Scheduling

A basic run queue and MCS lock based scheduler are provided in
`kern/sched.c`.  POSIX process helpers (`posix_spawn`, `posix_waitpid`)
are implemented in [`posix.c`](../posix.c).
Future steps:

* Expand the scheduler to support multiple cores and preemption.
* Track per‑process resources in capability objects for later revocation.
* Provide higher level spawn/exec wrappers with capability checks.

## Phase IV – Inter‑Process Communication

`docs/IPC.md` describes a mailbox mechanism and capability backed POSIX
IPC wrappers.  The implementation lives in [`examples/ipc/ipc.c`](../examples/ipc/ipc.c) and
corresponding headers under `include/`.
Planned improvements include:

* Complete FIFO support and integrate with the virtual file system.
* Extend message queue, semaphore and shared memory wrappers with
  persistent capability storage.
* Develop stress and security tests for the mailbox system.

## Phase V – File System and I/O

The modern tree implements simple FIFO handling in
`libposix/posix.c`.  Documentation for POSIX helper functions resides in
[`docs/POSIX.md`](POSIX.md).
Outstanding tasks:

* Design a capability‑aware VFS layer enforcing fine‑grained rights.
* Implement directory operations (`opendir`, `readdir`, `closedir`) and
  integrate them with capability checks.
* Provide atomic file operations and integrity verification.

## Phase VI – Networking

Networking wrappers are minimal.  Future work mirrors the roadmap’s
socket API with EINTR handling and IPv4 helpers.  Integration tests and
security checks are still required.

## Phase VII – Threading

`include/pthread.h` and `posix/pthread.c` implement a small subset of
POSIX threads backed by Mach cthreads.  As noted in
[`docs/posix_compat.md`](posix_compat.md) only creation, joining,
detaching and mutexes are available.
Planned extensions:

* Add condition variables and a capability‑aware thread cleanup model.
* Document limitations of the process‑based threading approach.

## Phase VIII – Timers and Synchronisation

Timer helpers such as `px_nanosleep` are planned but not yet present.
Kernel time handling currently uses placeholders (see
[`MODERNIZATION.md`](MODERNIZATION.md) lines 38‑42).  Work items:

* Implement per‑process timer accounting and high resolution sleeps.
* Ensure capability validation for timer creation and expiration.

## Phase IX – Testing Framework

Numerous unit tests live under `tests/`, covering capabilities,
IPC, IOMMU and POSIX helpers.  The roadmap envisions a broader automated
regression suite:

* Expand coverage to memory and process management.
* Provide integration tests spanning multiple subsystems.
* Generate coverage reports as part of the build.

## Phase X – Documentation and Deployment

Existing documents explain helper APIs and security demos.  The final
phase will produce comprehensive user guides and deployment tooling.
Automation scripts in `scripts/` (for example `run-qemu.sh`) already
assist with local testing.

---

This roadmap should evolve alongside the modernization notes to track
progress toward a secure, capability-enforced POSIX environment.  Formal proofs of the namespace algebra and IPC layers will be added as they mature.
