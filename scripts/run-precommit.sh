#!/usr/bin/env bash
set -euo pipefail

if command -v pre-commit >/dev/null 2>&1; then
  exec pre-commit "$@"
else
  echo "pre-commit is not installed." >&2
  echo "Install it with 'pip install pre-commit' and re-run this script." >&2
  exit 1
fi
