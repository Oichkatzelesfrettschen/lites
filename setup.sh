#!/usr/bin/env bash
#===============================================================================
# Title:   environment-setup.sh
# Purpose: Install and configure all required packages for development.
# Usage:   ./environment-setup.sh [--offline] [--help]
# Options:
#   --offline   Install from local .deb packages (offline mode)
#   --help      Show this help message and exit
#===============================================================================
set -Eeuo pipefail
trap 'log_failure "Failure at line $LINENO: $BASH_COMMAND"' ERR

#-------------------------------------------------------------------------------
# Globals
#-------------------------------------------------------------------------------
LOG="/tmp/setup.log"
FAIL_LOG="/tmp/setup_failures.log"
OFFLINE=false
OFFLINE_DIR="offline_packages"

#-------------------------------------------------------------------------------
# Logging functions
#-------------------------------------------------------------------------------
log_info()   { echo "[INFO]    $*" | tee -a "$LOG"; }
log_error()  { echo "[ERROR]   $*" | tee -a "$FAIL_LOG" >&2; }
log_failure(){ echo "[FATAL]   $*" | tee -a "$FAIL_LOG" >&2; exit 1; }

# Redirect all stdout/stderr to log
exec > >(tee -a "$LOG") 2>&1
set -x

export DEBIAN_FRONTEND=noninteractive

#-------------------------------------------------------------------------------
# Usage
#-------------------------------------------------------------------------------
usage() {
  cat <<EOF
Usage: $(basename "$0") [--offline] [--help]

Options:
  --offline   Use local .deb packages under '$OFFLINE_DIR'
  --help      Display this help and exit
EOF
  exit 0
}

#-------------------------------------------------------------------------------
# Parse options
#-------------------------------------------------------------------------------
while (( $# )); do
  case "$1" in
    --offline) OFFLINE=true; shift ;;
    --help)    usage      ;;
    *)         log_error "Unknown option: $1"; usage ;;
  esac
done

#-------------------------------------------------------------------------------
# Apt update & upgrade
#-------------------------------------------------------------------------------
if ! apt-get update -y; then
  log_error "apt-get update failed"
fi
if ! apt-get dist-upgrade -y; then
  log_error "apt-get dist-upgrade failed"
fi

#-------------------------------------------------------------------------------
# Clone or install OpenMach
#-------------------------------------------------------------------------------
OPENMACH_REPO=${OPENMACH_REPO:-https://github.com/machkernel/openmach.git}
if $OFFLINE; then
  log_info "Offline mode enabled"
  if compgen -G "${OFFLINE_DIR}/*.deb" > /dev/null; then
    dpkg -i "${OFFLINE_DIR}"/*.deb || log_error "dpkg install failed"
  else
    log_info "No .deb files found in '${OFFLINE_DIR}'"
  fi
else
  if [[ ! -d openmach ]]; then
    if git clone "$OPENMACH_REPO" openmach; then
      log_info "Cloned OpenMach from $OPENMACH_REPO"
    else
      log_error "Failed to clone OpenMach"
    fi
  else
    log_info "OpenMach directory already exists"
  fi
fi

#-------------------------------------------------------------------------------
# install_pkg: try apt, pip, npm, then custom handlers
#-------------------------------------------------------------------------------
install_pkg() {
  local pkg="$1"
  log_info "Installing package: $pkg"

  if apt-get install -y "$pkg"; then
    log_info "$pkg installed via apt"
    return 0
  fi
  log_error "apt-get failed for $pkg"

  if pip install "$pkg"; then
    log_info "$pkg installed via pip"
    return 0
  fi
  log_error "pip failed for $pkg"

  if npm install -g "$pkg"; then
    log_info "$pkg installed via npm"
    return 0
  fi
  log_error "npm failed for $pkg"

  case "$pkg" in
    shellcheck)
      wget -qO- \
        https://github.com/koalaman/shellcheck/releases/download/v0.9.0/\
shellcheck-v0.9.0.linux.x86_64.tar.xz | tar xJ
      install -m 0755 shellcheck-v0.9.0/shellcheck /usr/local/bin/shellcheck
      rm -rf shellcheck-v0.9.0
      log_info "shellcheck installed from binary"
      return 0
      ;;
    capnproto)
      wget -qO- \
        https://github.com/capnproto/capnproto/archive/refs/tags/v0.9.1.tar.gz \
        | tar xz
      pushd capnproto-0.9.1 >/dev/null
      ./setup-makefiles.sh && ./configure && make -j"$(nproc)" && make install
      popd >/dev/null
      log_info "capnproto built from source"
      return 0
      ;;
    isabelle)
      wget -qO- \
        https://isabelle.in.tum.de/dist/Isabelle2023-1_linux.tar.gz | tar xz
      install -d /opt/isabelle
      mv Isabelle2023-1 /opt/isabelle
      ln -sf /opt/isabelle/Isabelle2023-1/bin/isabelle /usr/local/bin/isabelle
      log_info "isabelle installed from archive"
      return 0
      ;;
    *)
      log_error "No handler for package '$pkg'"
      return 1
      ;;
  esac
}

#-------------------------------------------------------------------------------
# Package list
#-------------------------------------------------------------------------------
packages=(
  build-essential git wget curl
  clang lld llvm-dev libclang-dev polly
  clang-tools clang-tidy clang-format clangd
  ccache lldb gdb bolt llvm-bolt
  cmake make ninja-build meson
  doxygen graphviz python3-sphinx python3-breathe
  shellcheck yamllint
  python3 python3-pip python3-venv python3-setuptools python3-wheel
  nodejs npm yarnpkg
  coq coqide tla4tools isabelle
  afl++ honggfuzz cargo-fuzz
  # QEMU and debugging tools for running the OS in emulation
  qemu-system-x86 qemu-system-x86_64 qemu-system-i386 qemu-utils valgrind lcov gcovr
  tmux cloc libperl-dev gdb-multiarch
)

#-------------------------------------------------------------------------------
# Install all packages
#-------------------------------------------------------------------------------
for pkg in "${packages[@]}"; do
  install_pkg "$pkg" || true
done

#-------------------------------------------------------------------------------
# Summarize & auto-repair failures
#-------------------------------------------------------------------------------
if [[ -s "$FAIL_LOG" ]]; then
  log_info "Packages failed to install:"
  sed 's/^/[FAIL] /' "$FAIL_LOG" | tee -a "$LOG"
  log_info "Attempting automatic repair (apt-get -f install)"
  if apt-get -f install -y >> "$LOG" 2>&1; then
    : > "$FAIL_LOG"
    log_info "Automatic repair succeeded"
  else
    log_error "Automatic repair failed; inspect $FAIL_LOG"
  fi
else
  log_info "All packages installed successfully"
fi

#-------------------------------------------------------------------------------
# Environment exports
#-------------------------------------------------------------------------------
export CC="ccache clang"
export CXX="ccache clang++"
export CLANG_TIDY="clang-tidy"
export PATH="/usr/lib/ccache:$PATH"
export CFLAGS="-Wall -Wextra -Werror -O2"
export CXXFLAGS="$CFLAGS"
# Harden binaries by disallowing executable stacks
# Harden binaries by disallowing executable stacks. Add the flag to
# existing options if needed.
export LDFLAGS="-fuse-ld=lld -flto ${LDFLAGS:+$LDFLAGS }-Wl,-z,noexecstack"
export LLVM_PROFILE_FILE="/tmp/profiles/default.profraw"
export CLANG_EXTRA_FLAGS="-mllvm -polly"

log_info "Environment setup complete."
