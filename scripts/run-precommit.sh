#!/usr/bin/env bash
set -euo pipefail


command_exists() { command -v "$1" >/dev/null 2>&1; }

has_network() {
  ping -c1 -W1 8.8.8.8 >/dev/null 2>&1
}

if ! command_exists pre-commit; then
  if ! command_exists pip; then
    echo "pip is not available to install pre-commit." >&2
    exit 1
  fi

  if has_network; then
    echo "pre-commit not found; installing via pip..." >&2
    if ! pip install --user --quiet pre-commit; then
      echo "Failed to install pre-commit via pip." >&2
      exit 1
    fi
  else
    echo "Network unavailable. Please run setup.sh or install pre-commit manually." >&2
    exit 1
  fi
fi

exec pre-commit "$@"
