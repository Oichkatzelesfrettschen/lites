#!/usr/bin/env bash
set -euo pipefail

## \file tmux-qemu.sh
## \brief Launch tmux with run-qemu.sh and an interactive shell.
##
## The script spawns a tmux session running \c run-qemu.sh in one pane and
## opens a second pane with a standard shell for interactive use.
##
## \param [1] architecture Target architecture passed to \c run-qemu.sh.
##                         Defaults to \c x86_64 when unspecified.

arch="${1:-x86_64}"
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
qemu_script="$script_dir/run-qemu.sh"

if ! command -v tmux >/dev/null 2>&1; then
  echo "tmux not found in PATH." >&2
  exit 1
fi

if [ ! -x "$qemu_script" ]; then
  echo "$qemu_script not found. Build with Makefile.new." >&2
  exit 1
fi

session="lites"

tmux new-session -d -s "$session" "$qemu_script" "$arch"
tmux split-window -v -t "$session" "$SHELL"
tmux select-pane -t "$session":0.0
exec tmux attach-session -t "$session"