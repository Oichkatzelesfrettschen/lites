#!/usr/bin/env bash
##
# @file tmux-qemu.sh
# @brief Start a tmux session with QEMU and a shell.
#
# This helper spawns a two-pane tmux session. The left pane executes
# `scripts/run-qemu.sh` with the selected architecture, while the right
# pane provides an interactive shell. The session is named `lites`.
#
# @usage scripts/tmux-qemu.sh [arch]
# @param arch Optional architecture passed to `run-qemu.sh`. Defaults to
#             `x86_64` when omitted.
##

set -euo pipefail

# Architecture to boot via run-qemu.sh
arch="${1:-x86_64}"
# Name of the tmux session to create
session="lites"

##
# @brief Launch tmux with QEMU in one pane and a shell in the other.
#
# @param arch    Architecture argument for run-qemu.sh.
# @param session Name of the tmux session.
#
# @return 0 on success.
##
start_session() {
  local arch="$1"
  local session="$2"

  # Launch QEMU in a detached tmux session
  tmux new-session -d -s "$session" "scripts/run-qemu.sh \"$arch\""
  # Split the window to open a shell beside QEMU
  tmux split-window -h
  # Attach to the session so the user can interact
  tmux attach -t "$session"
}

start_session "$arch" "$session"
