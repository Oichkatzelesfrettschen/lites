#!/usr/bin/env bash
set -euo pipefail

arch="${1:-x86_64}"
server="lites_server"
emulator="lites_emulator"

case "$arch" in
  i686)
    qemu_cmd="qemu-system-i386"
    ;;
  x86_64)
    qemu_cmd="qemu-system-x86_64"
    ;;
  *)
    echo "Usage: $0 [i686|x86_64]" >&2
    exit 1
    ;;
esac

if [ ! -x "$server" ]; then
  echo "$server not found. Build with Makefile.new first." >&2
  exit 1
fi

if ! command -v "$qemu_cmd" >/dev/null 2>&1; then
  echo "$qemu_cmd not found in PATH." >&2
  exit 1
fi

opts=(-nographic -kernel "$server")
if [ -x "$emulator" ]; then
  opts+=(-initrd "$emulator")
fi

echo "Launching $server using $qemu_cmd for $arch" >&2
exec "$qemu_cmd" "${opts[@]}"
