#!/usr/bin/env bash
# =============================================================================
# Title:   clang-shim.sh
# Purpose: Provide wrapper symlinks so that cc and c++ invoke clang by default.
# Usage:   ./clang-shim.sh <clang-version>
#
# The script creates /usr/local/bin/cc and /usr/local/bin/c++ as symlinks
# pointing to clang and clang++ of the specified version. The symlinks are
# recreated when they already exist.
# =============================================================================
set -euo pipefail

if [[ $# -ne 1 ]]; then
    echo "Usage: $(basename "$0") <clang-version>" >&2
    exit 1
fi

clang_ver=$1
clang_bin="/usr/bin/clang-${clang_ver}"
clangpp_bin="/usr/bin/clang++-${clang_ver}"

if [[ ! -x $clang_bin || ! -x $clangpp_bin ]]; then
    echo "clang version ${clang_ver} not found" >&2
    exit 1
fi

install -d /usr/local/bin
ln -sf "$clang_bin" /usr/local/bin/cc
ln -sf "$clangpp_bin" /usr/local/bin/c++

exit 0

