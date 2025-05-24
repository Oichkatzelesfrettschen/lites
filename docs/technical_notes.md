# Technical Notes

The included PDF, `johannes_helander-unix_under_mach-the_lites_server.pdf`,
contains the master's thesis *Unix Under Mach – The Lites Server*. It describes
Lites' architecture and how BSD was adapted to run on Mach. The PostScript
`lites.MASTERS.ps` is another format of the same document.

Building older releases often requires a matching Mach kernel. The Utah Flux
project hosted detailed instructions and patches, many of which are mirrored at
[archive.org](https://web.archive.org/).

To experiment with cross compilation or multiple architectures, see the
`setup.sh` script in the repository root. It installs a full toolchain and
qemu-based emulation targets.

The modernized sources in `src-lites-1.1-2025` require a compiler with full
C23 support. GCC 13 or Clang 17 (or newer) are known to work.


Source files are formatted with `clang-format` using the settings in `.clang-format`. The `scripts/format-code.sh` helper applies the formatter to all tracked C and C++ files.

A repository-wide `.editorconfig` enforces UTF‑8 encoding, LF line endings and four-space indentation.

## User-level pager

`src-lites-1.1-2025/bin/user_pager` implements a trivial pager used by the unit
tests.  It reads `pf_info_t` structures from a UNIX domain socket and simply
acknowledges each request, leaving the kernel side to map zero-filled pages.
The VM tests spawn the pager automatically.
