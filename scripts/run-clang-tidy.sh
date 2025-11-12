#!/usr/bin/env bash
set -euo pipefail

# Ensure build environment is set up
if [ -f "scripts/setup-build-environment.sh" ]; then
  source scripts/setup-build-environment.sh >/dev/null 2>&1 || true
fi

# Ensure a compile_commands.json exists
compdb="compile_commands.json"
if [ ! -f "$compdb" ]; then
  echo "[run-clang-tidy] Generating $compdb via CMake" >&2
  if cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON >/dev/null 2>&1; then
    if [ -f build/compile_commands.json ]; then
      ln -sf build/compile_commands.json "$compdb"
    fi
  else
    echo "[run-clang-tidy] Warning: failed to generate compilation database" >&2
    echo "[run-clang-tidy] Skipping clang-tidy checks" >&2
    exit 0
  fi
fi

# If no compilation database, skip clang-tidy
if [ ! -f "$compdb" ] && [ ! -f "build/$compdb" ]; then
  echo "[run-clang-tidy] No compilation database found, skipping" >&2
  exit 0
fi

exec clang-tidy -p build "$@"
