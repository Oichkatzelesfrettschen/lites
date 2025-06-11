#!/usr/bin/env bash
##
# @file tmux-qemu.sh
# @brief Launch QEMU inside tmux alongside an interactive shell.
#
# Usage:
#   scripts/tmux-qemu.sh [arch]
#
# The script starts a tmux session with two panes. The left pane runs
# `scripts/run-qemu.sh` for the selected architecture, while the right
# pane offers a regular shell. The architecture defaults to `x86_64`.
##
set -euo pipefail

##
# @brief Launch QEMU inside a tmux session.
#
# @param arch Target architecture. Defaults to `x86_64` when unspecified.
# @return void
##
main() {
  local arch="${1:-x86_64}"
  local session="lites"

  # Start the session detached with QEMU running in the first pane.
  tmux new-session -d -s "$session" "scripts/run-qemu.sh \"$arch\""
  # Add a second pane for an interactive shell.
  tmux split-window -h
  # Attach to the new session so the user can interact with both panes.
  tmux attach -t "$session"
}

main "$@"
