# Documentation Index

The `docs/` directory contains notes and background information for the project. Below is a list of each file with a short description.

- **HISTORY.md** - Overview of available Lites releases and helper script for generating diffs between them.
- **IPC.md** - Describes the mailbox-based interprocess communication helpers.
- **POSIX.md** - Documents small convenience wrappers for POSIX operations.
- **POSIX_ROADMAP.md** - High level plan for implementing POSIX features with
  links into the specification under `docs/books/`.
- Historical planning documents have been removed as the repository now reflects the consolidated modern tree.
- **iommu.md** - Notes on the minimal IOMMU abstraction present in the source tree.
- **posix_compat.md** - Explains the limited POSIX threads compatibility layer.
- **security.md** - Outlines the keystore and enclave security APIs.
- **LICENSE_MAP** - Produced by `scripts/gen-license-map.sh` (not version-controlled); maps each source to its license header.
- **technical_notes.md** - Additional technical notes and links to the 'Unix Under Mach' thesis.
- **tmux.md** - Suggested tmux workflow and QEMU troubleshooting tips.
- **ARCHITECTURE.md** - Modern architecture, design and implementation
  roadmap summarising the project state and integration of third party
  sources such as `asmlib`. Includes statistics from `cloc`, `lizard` and
  `cscope`.
- **archives/** - Preserved snapshots, old build files such as
  `makefile.legacy`, and tarballs from earlier releases.
- **examples/** - Demonstration programs grouped by topic (`ipc/`, `posix/`,
  `security/`).
- **util/** - Small command line utilities and helper tools.
