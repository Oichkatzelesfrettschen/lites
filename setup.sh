#!/usr/bin/env bash
set -euo pipefail
export DEBIAN_FRONTEND=noninteractive
apt-get update -y

# Base compiler toolchain and build utilities
apt-get install -y \
  clang lld llvm llvm-dev libclang-dev \
  clang-tools clang-tidy clang-format clangd \
  ccache lldb gdb bolt llvm-bolt \
  cmake make ninja-build \
  shellcheck yamllint

# Additional languages and package managers
apt-get install -y \
  python3 python3-pip python3-venv python3-setuptools python3-wheel \
  nodejs npm yarnpkg

# Formal verification tools
apt-get install -y \
  coq coqide tla4tools

# Fuzzing frameworks
apt-get install -y \
  afl++ honggfuzz cargo-fuzz

# Set clang as default C/C++ compiler via ccache
export CC="ccache clang"
export CXX="ccache clang++"
export CLANG_TIDY=clang-tidy
export PATH="/usr/lib/ccache:$PATH"

# Recommended compilation and linter flags
export CFLAGS="-Wall -Wextra -Werror -O2"
export CXXFLAGS="$CFLAGS"
export LDFLAGS="-fuse-ld=lld -flto"
