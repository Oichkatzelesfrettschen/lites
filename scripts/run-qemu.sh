#!/usr/bin/env bash
set -euo pipefail

##
# @brief Target architecture passed to the script. Defaults to x86_64.
##
arch="${1:-x86_64}"

##
# @brief Name of the primary server binary.
##
server="lites_server"

##
# @brief Name of the optional emulator binary.
##
emulator="lites_emulator"

##
# @brief Print usage information and exit with an error code.
#
# @return Does not return; exits the process.
##
print_usage() {
  echo "Usage: $0 [i686|x86_64]" >&2
  exit 1
}

##
# @brief Resolve the path to a binary.
#
# The function first checks for the binary in the current working
# directory and falls back to the CMake build directory if needed.
#
# @param name The basename of the binary to locate.
#
# @return Prints the resolved path if successful; exits otherwise.
##
##
# @brief Display instructions on building the required binaries.
#
# This helper hints at the typical commands needed to compile
# `lites_server` and its companion tools when they are absent.
# It intentionally emits the message to standard error so that
# the invocation of this script clearly fails with guidance.
#
# @return Nothing. Prints to stderr.
##
print_build_hint() {
  cat >&2 <<'EOF'
Run ./setup.sh to install dependencies and clone OpenMach.
Then build using:
  make -f Makefile.new ARCH=i686 \
      LITES_MACH_DIR=openmach \
      SRCDIR=Items1/lites-1.1.u3
EOF
}

find_binary() {
  local name=$1
  local candidates=("$name" "build/$name")

  for candidate in "${candidates[@]}"; do
    if [[ -x $candidate ]]; then
      echo "$candidate"
      return 0
    fi
  done

  echo "$name not found." >&2
  print_build_hint
  exit 1
}

##
# @brief Resolve the path to an optional binary.
#
# The search order matches that of find_binary, but failure does not exit the
# script.
#
# @param name Binary name to locate.
#
# @return Prints the resolved path if found. Returns 1 when absent.
##
find_optional_binary() {
  local name=$1
  local path
  if path=$(find_binary "$name" 2>/dev/null); then
    echo "$path"
    return 0
  fi
  return 1
}

##
# @brief Determine the qemu-system command based on the architecture.
#
# @param arch Target architecture (i686 or x86_64).
#
# @return Prints the qemu-system executable name if found; exits otherwise.
##
select_qemu() {
  local arch=$1
  case "$arch" in
    i686)
      echo "qemu-system-i386"
      ;;
    x86_64)
      echo "qemu-system-x86_64"
      ;;
    *)
      print_usage
      ;;
  esac
}

##
# @brief Run QEMU with the provided architecture.
#
# @param arch Architecture to emulate.
##
run_qemu() {
  local arch=$1
  local qemu_cmd
  qemu_cmd=$(select_qemu "$arch")

  if ! command -v "$qemu_cmd" >/dev/null 2>&1; then
    echo "$qemu_cmd not found in PATH. Run ./setup.sh to install QEMU." >&2
    exit 1
  fi

  local server_path emulator_path
  server_path=$(find_binary "$server")
  emulator_path=$(find_optional_binary "$emulator" || true)

  local opts=(-nographic -kernel "$server_path")
  if [[ -n "$emulator_path" ]]; then
    opts+=(-initrd "$emulator_path")
  fi

  echo "Launching $server_path using $qemu_cmd for $arch" >&2
  exec "$qemu_cmd" "${opts[@]}"
}

run_qemu "$arch"
