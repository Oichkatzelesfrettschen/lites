# Lites Scripts

This directory contains utility scripts for building, testing, and maintaining the Lites BSD server.

## Script Categories

### Build Tools

**check_build_tools.sh**
- **WHY**: Verify all required build dependencies are installed before building
- **WHAT**: Checks for GCC/Clang, CMake, MIG, Mach headers, and other tools
- **HOW**: `./check_build_tools.sh`
- **Returns**: 0 if all tools present, 1 if any missing (with detailed report)

**setup-build-environment.sh**
- **WHY**: Configure build environment variables and paths
- **WHAT**: Sets LITES_MACH_DIR, PATH for MIG, and other build variables
- **HOW**: `source ./setup-build-environment.sh` (note: must source, not execute)
- **Effect**: Modifies current shell environment

**clang-shim.sh**
- **WHY**: Wrap Clang to add Mach-specific compiler flags
- **WHAT**: Compiler shim that adds -I paths and Mach-specific defines
- **HOW**: Set CC=./clang-shim.sh in CMake or Makefile
- **Use Case**: When building with Clang instead of Mach-patched GCC

### Code Quality

**format-code.sh**
- **WHY**: Ensure consistent code formatting across the codebase
- **WHAT**: Runs clang-format on all C source files
- **HOW**: `./format-code.sh` (formats in-place) or `./format-code.sh --check` (dry-run)
- **Config**: Uses .clang-format in repository root

**run-clang-tidy.sh**
- **WHY**: Catch bugs and enforce coding standards with static analysis
- **WHAT**: Runs clang-tidy with Lites-specific checks
- **HOW**: `./run-clang-tidy.sh [path]` (default: entire src/ tree)
- **Config**: Uses .clang-tidy configuration

**run-precommit.sh**
- **WHY**: Run all quality checks before committing to catch issues early
- **WHAT**: Executes format-code.sh --check, run-clang-tidy.sh, and unit tests
- **HOW**: `./run-precommit.sh` (should be run before every commit)
- **Git Hook**: Can be symlinked as .git/hooks/pre-commit

### Header Management

**extract-xmach-headers.sh**
- **WHY**: Extract Mach kernel headers from xMach distribution
- **WHAT**: Unpacks and organizes headers from xMach tarball
- **HOW**: `./extract-xmach-headers.sh /path/to/xmach.tar.gz`
- **Output**: Headers placed in include/mach/

**flatten-headers.sh**
- **WHY**: Resolve #include dependency chains to single-file headers
- **WHAT**: Recursively inlines #include directives
- **HOW**: `./flatten-headers.sh input.h > output-flat.h`
- **Use Case**: Creating self-contained header files for distribution

**import-mach-headers.sh**
- **WHY**: Import Mach headers from another source (OpenMach, OSF MK)
- **WHAT**: Copies headers while preserving directory structure
- **HOW**: `./import-mach-headers.sh /path/to/mach/include`
- **Safety**: Creates backups before overwriting

**move_all_headers.sh**
- **WHY**: Reorganize header files into standard include/ hierarchy
- **WHAT**: Moves scattered headers to include/{mach,sys,bsd}/
- **HOW**: `./move_all_headers.sh` (interactive prompts for conflicts)
- **Warning**: Modifies file layout; commit before running

### Analysis and Documentation

**trace_graph.py**
- **WHY**: Visualize call graphs and function dependencies
- **WHAT**: Generates GraphViz DOT files from C source analysis
- **HOW**: `python trace_graph.py src/ > callgraph.dot && dot -Tpng -o graph.png callgraph.dot`
- **Requires**: Python 3.7+, pycparser

**include_graph.sh**
- **WHY**: Map header inclusion dependencies to detect cycles
- **WHAT**: Creates dependency graph of #include relationships
- **HOW**: `./include_graph.sh > includes.dot`
- **Output**: GraphViz format showing which headers include which

**gen-license-map.sh**
- **WHY**: Generate SPDX license manifest for all source files
- **WHAT**: Scans files for license headers and creates mapping
- **HOW**: `./gen-license-map.sh > LICENSE-MAP.txt`
- **Use Case**: Compliance audits and license documentation

**generate-diffs.sh**
- **WHY**: Create unified diffs between Lites versions for comparison
- **WHAT**: Generates diff output for source evolution analysis
- **HOW**: `./generate-diffs.sh lites-1.1 lites-1.1.u3`
- **Output**: Diff files in diffs/ directory

### Testing and Development

**run-qemu.sh**
- **WHY**: Boot Lites in QEMU emulator for testing
- **WHAT**: Launches QEMU with appropriate kernel and disk images
- **HOW**: `./run-qemu.sh [--debug] [--serial]`
- **Options**: --debug (GDB stub), --serial (serial console output)

**tmux-qemu.sh**
- **WHY**: Run QEMU in tmux session for persistent testing
- **WHAT**: Starts QEMU in detached tmux with multiple panes
- **HOW**: `./tmux-qemu.sh session-name`
- **Attach**: `tmux attach -t session-name`

### Utility Scripts

**configuredb.sh**
- **WHY**: Initialize SQLite database for build artifact tracking
- **WHAT**: Creates database schema for object files, dependencies
- **HOW**: `./configuredb.sh [database.db]`
- **Use Case**: Advanced build caching and analysis

**flatten_tree.sh**
- **WHY**: Create flat directory copy of source tree (all files in one dir)
- **WHAT**: Copies all source files to single output directory
- **HOW**: `./flatten_tree.sh src/ output/`
- **Use Case**: Feeding to single-directory analysis tools

**unify_same_name.sh**
- **WHY**: Detect and optionally merge files with same basename
- **WHAT**: Finds files like src/foo.c and lib/foo.c
- **HOW**: `./unify_same_name.sh [--merge]`
- **Warning**: Use --merge cautiously; it modifies files

## Usage Examples

### Before First Build
```bash
./check_build_tools.sh
source ./setup-build-environment.sh
cd ../build
cmake ..
```

### Daily Development Workflow
```bash
# Make changes to code
vi ../src/server/syscalls.c

# Check code quality
./format-code.sh --check
./run-clang-tidy.sh ../src/server/

# Run tests
./run-precommit.sh

# If all passes, commit
git add ../src/server/syscalls.c
git commit -m "fix(syscalls): handle EINTR properly"
```

### Importing New Mach Headers
```bash
./import-mach-headers.sh ../../openmach/include
# Verify changes
git diff ../include/
# If good, commit
git add ../include/
git commit -m "chore: update Mach headers from OpenMach"
```

### Testing in QEMU
```bash
# Build kernel
cd ../build && make -j$(nproc)

# Boot in QEMU (serial console)
cd ../scripts
./run-qemu.sh --serial

# Or in tmux for long-running tests
./tmux-qemu.sh test-session
tmux attach -t test-session
```

## Adding New Scripts

When adding a new script to this directory:

1. **Name it descriptively**: Use verb-noun format (e.g., check-headers.sh, not headers.sh)
2. **Add shebang**: Start with `#!/bin/bash` or `#!/usr/bin/env python3`
3. **Make it executable**: `chmod +x script-name.sh`
4. **Add usage message**: Implement --help flag
5. **Document here**: Add entry to this README with WHY/WHAT/HOW
6. **Follow standards**: Use shellcheck for bash, black/flake8 for Python
7. **Error handling**: Use `set -euo pipefail` for bash scripts

## Script Maintenance

- **Review quarterly**: Remove unused scripts, update documentation
- **Test before committing**: All scripts should work on clean checkout
- **Keep dependencies minimal**: Prefer POSIX sh over bash, avoid exotic tools
- **Document breaking changes**: Update CHANGELOG if script behavior changes

---

Last updated: 2025-11-14
Maintained by: Lites development team
