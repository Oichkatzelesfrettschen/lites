# Lites Archives

This directory contains archived historical materials and legacy build artifacts.

## Directory Structure

### historical-releases/
Historical Lites versions and original documentation.

**Contents:**
- lites-1.0 - Original 1.0 release
- lites-1.1 - Version 1.1
- lites-1.1.1 - Bug-fix release
- lites-1.1-950808 - 1995-08-08 snapshot
- lites-1.1.u1, u2, u3 - Update releases
- PDF documentation from original distributions

**WHY**: Preserve complete release history for research and regression testing.

**WHAT**: Source tarballs, build scripts, and contemporary documentation.

**HOW**: Extract with `tar xzf <filename>` and refer to included READMEs.

### build-experiments/
Experimental build system configurations tried during modernization.

**Contents:**
- Alternative CMakeLists.txt variants
- Meson build experiments
- Legacy Makefile fragments

**WHY**: Document build system evolution and preserve working configurations
       that may be useful for specialized builds.

**WHAT**: Build configuration files from 2023-2025 modernization efforts.

**HOW**: Reference when troubleshooting build issues or trying alternative
       build approaches.

### legacy_build/
Archived legacy build system files (pre-CMake era).

**Contents:**
- Original Makefiles and build scripts
- Platform-specific build configurations
- Historical build documentation

**WHY**: Preserve original build methodology for historical accuracy and
       as fallback if modern build breaks.

**WHAT**: Complete original build system as of 2020.

**HOW**: See legacy_build/README for original build instructions.

## Usage Guidelines

**DO NOT MODIFY** files in these archives. They are preserved for historical
reference and research.

**TO USE**: Extract to a working directory outside archives/:
```bash
cd /tmp
tar xzf /path/to/lites/archives/historical-releases/lites-1.1.tar.gz
cd lites-1.1/
# Build and experiment
```

**TO REFERENCE**: Read documentation or examine configurations without
extracting. Most .tar.gz files can be inspected with:
```bash
tar tzf filename.tar.gz  # List contents
tar xzf filename.tar.gz path/to/specific/file  # Extract one file
```

## Maintenance

- **DO NOT DELETE** any archived materials
- **ADD** new archive subdirectories as needed with descriptive names
- **UPDATE** this README when adding new archive categories
- **COMPRESS** large archive directories if they exceed 100 MB

---

Last updated: 2025-11-14
