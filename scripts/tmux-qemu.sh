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

arch="${1:-x86_64}"
session="lites"

# Start the session detached, running QEMU in the first pane.
tmux new-session -d -s "$session" "scripts/run-qemu.sh \"$arch\""
# Create a second pane for interactive commands.
tmux split-window -h
# Attach to the newly created session.
tmux attach -t "$session"
