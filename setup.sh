#!/usr/bin/env bash
set -euo pipefail
set -x
LOG_FILE="/tmp/setup.log"
FAIL_LOG="/tmp/setup_failures.log"
: > "$LOG_FILE"
: > "$FAIL_LOG"
exec > >(tee -a "$LOG_FILE") 2>&1

export DEBIAN_FRONTEND=noninteractive
apt-get update -y
apt-get dist-upgrade -y

install_pkg() {
  local pkg="$1"
  if ! apt-get install -y "$pkg"; then
    echo "apt-get failed for $pkg" >> "$FAIL_LOG"
    if ! pip install -q "$pkg"; then
      echo "pip failed for $pkg" >> "$FAIL_LOG"
      if ! npm install -g "$pkg" >/dev/null 2>&1; then
        echo "npm failed for $pkg" >> "$FAIL_LOG"
      fi
    fi
  fi
}

install_pkg_list() {
  for p in "$@"; do
    install_pkg "$p"
  done
}

# Base compiler toolchain and build utilities
install_pkg_list \
  build-essential gcc g++ m4 \
  clang lld llvm llvm-dev libclang-dev \
  clang-tools clang-tidy clang-format clangd \
  ccache lldb gdb bolt llvm-bolt \
  cmake make ninja-build meson \
  bmake mk-configure \
  shellcheck yamllint \
  qemu-system-x86 gcc-i686-linux-gnu g++-i686-linux-gnu

# Additional languages and package managers
install_pkg_list \
  python3 python3-pip python3-venv python3-setuptools python3-wheel \
  nodejs npm yarnpkg

# Formal verification tools
install_pkg_list \
  coq coqide tla4tools

# Fuzzing frameworks
install_pkg_list \
  afl++ honggfuzz cargo-fuzz

if ! command -v pre-commit >/dev/null 2>&1; then
  if ! pip install -q pre-commit; then
    echo "pre-commit installation failed" >> "$FAIL_LOG"
  fi
fi

# Set clang as default C/C++ compiler via ccache
export CC="ccache clang"
export CXX="ccache clang++"
export CLANG_TIDY=clang-tidy
export PATH="/usr/lib/ccache:$PATH"

# Recommended compilation and linter flags
export CFLAGS="-Wall -Wextra -Werror -O2"
export CXXFLAGS="$CFLAGS"
export LDFLAGS="-fuse-ld=lld -flto"
