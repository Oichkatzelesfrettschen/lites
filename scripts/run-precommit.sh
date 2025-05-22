#!/usr/bin/env bash
set -euo pipefail

if ! command -v pre-commit >/dev/null 2>&1; then
  echo "pre-commit not found; attempting installation via pip..." >&2
  if command -v pip >/dev/null 2>&1; then
    if ! pip install --user --quiet pre-commit; then
      echo "Failed to install pre-commit via pip." >&2
      exit 1
    fi
  else
    echo "pip is not available to install pre-commit." >&2
    exit 1
  fi
fi

exec pre-commit "$@"
