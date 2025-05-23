#!/usr/bin/env bash
set -euo pipefail

repo_root="$(git -C "$(dirname "$0")/.." rev-parse --show-toplevel)"
cd "$repo_root"

# Format all tracked C/C++ source files
git ls-files -z | grep -zE '\.(c|h|cpp|hpp|cc)$' | \
  xargs -0 clang-format -i --style=file
