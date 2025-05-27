# Repository Restructure Goals

The repository is being reorganised to keep historical snapshots
alongside a modernised tree without mixing the two.  A separate
`lites-1.1-2025` directory holds the evolving sources while the
original archives remain untouched.  Common build files and tools are
located at the repository root so that all architectures share the
same layout.

Main objectives:

- preserve legacy releases for reference
- consolidate documentation and helper scripts
- support out-of-tree builds via `LITES_SRC_DIR`
- keep a unified directory structure for 32‑bit and 64‑bit targets
- simplify integration with external Mach kernels and optional
  libraries

The restructure makes it easier to compare historic code with the
modern effort while ensuring new features do not pollute the archived
sources.
