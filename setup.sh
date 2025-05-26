#!/usr/bin/env bash
set -Eeuo pipefail
trap 'echo "Failure at line $LINENO: $BASH_COMMAND" >&2' ERR
exec > >(tee -a setup.log) 2>&1
set -x

export DEBIAN_FRONTEND=noninteractive

apt-get update -y
apt-get dist-upgrade -y

install_pkg() {
  local pkg="$1"
  if apt-get install -y "$pkg"; then
    return 0
  fi
  echo "apt-get failed for $pkg" >&2
  if pip install "$pkg"; then
    return 0
  fi
  echo "pip failed for $pkg" >&2
  if npm install -g "$pkg"; then
    return 0
  fi
  echo "npm failed for $pkg" >&2
  if [[ "$pkg" == "shellcheck" ]]; then
    wget -qO- https://github.com/koalaman/shellcheck/releases/download/v0.9.0/shellcheck-v0.9.0.linux.x86_64.tar.xz | tar xJ
    install -m 0755 shellcheck-v0.9.0/shellcheck /usr/local/bin/shellcheck
    rm -rf shellcheck-v0.9.0
    return 0
  fi
  if [[ "$pkg" == "capnproto" ]]; then
    wget -qO- https://github.com/capnproto/capnproto/archive/refs/tags/v0.9.1.tar.gz | tar xz
    pushd capnproto-0.9.1
    ./setup-makefiles.sh
    ./configure
    make -j"$(nproc)"
    make install
    popd
    return 0
  fi
  if [[ "$pkg" == "isabelle" ]]; then
    wget -qO- https://isabelle.in.tum.de/dist/Isabelle2023-1_linux.tar.gz | tar xz
    install -d /opt/isabelle
    mv Isabelle2023-1 /opt/isabelle
    ln -s /opt/isabelle/Isabelle2023-1/bin/isabelle /usr/local/bin/isabelle
    return 0
  fi
  if [[ "$pkg" == "asda" ]]; then
    wget -qO /usr/local/bin/asda https://example.com/asda
    chmod +x /usr/local/bin/asda
    return 0
  fi
  echo "could not install $pkg" >&2
}

packages=(
  build-essential git wget curl
  clang lld llvm llvm-dev libclang-dev polly
  clang-tools clang-tidy clang-format clangd
  ccache lldb gdb bolt llvm-bolt
  cmake make ninja-build
  doxygen graphviz python3-sphinx
  shellcheck yamllint
  python3 python3-pip python3-venv python3-setuptools python3-wheel
  nodejs npm yarnpkg
  coq coqide tla4tools isabelle asda
  afl++ honggfuzz cargo-fuzz
)

for pkg in "${packages[@]}"; do
  install_pkg "$pkg"
done

export CC="ccache clang"
export CXX="ccache clang++"
export CLANG_TIDY=clang-tidy
export PATH="/usr/lib/ccache:$PATH"

export CFLAGS="-Wall -Wextra -Werror -O2"
export CXXFLAGS="$CFLAGS"
export LDFLAGS="-fuse-ld=lld -flto"
export LLVM_PROFILE_FILE="/tmp/profiles/default.profraw"
export CLANG_EXTRA_FLAGS="-mllvm -polly"
