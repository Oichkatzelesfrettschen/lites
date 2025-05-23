#!/usr/bin/env bash
set -euo pipefail

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
  fi
fi

exec clang-tidy -p build "$@"
