#!/usr/bin/env bash
set -euo pipefail
export DEBIAN_FRONTEND=noninteractive
apt-get update -y
apt-get install -y clang lld llvm
# set clang as default C compiler
export CC=clang
export CXX=clang++
