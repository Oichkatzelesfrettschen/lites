#!/usr/bin/env bash
set -euo pipefail
export DEBIAN_FRONTEND=noninteractive
# log file for any package installation failures
FAIL_LOG=/tmp/setup_failures.log
: > "$FAIL_LOG"

#— helper to pin to the repo’s exact version if it exists
apt_pin_install(){
  pkg="$1"
  ver=$(apt-cache show "$pkg" 2>/dev/null \
        | awk '/^Version:/{print $2; exit}')
  # Attempt to install the exact version if available. Failures are logged but
  # do not abort the overall setup so subsequent installation methods may run.
  if [ -n "$ver" ]; then
    apt_pkg="${pkg}=${ver}"
  else
    apt_pkg="$pkg"
  fi

  if ! apt-get install -y "$apt_pkg"; then
    echo "apt:$pkg" >> "$FAIL_LOG"
    # if this looks like a python package try pip
    if [[ "$pkg" == python3-* ]]; then
      pip_pkg="${pkg#python3-}"
      if ! pip3 install --no-cache-dir "$pip_pkg"; then
        echo "pip:$pip_pkg" >> "$FAIL_LOG"
      fi
    fi
  fi
}

#— enable foreign architectures for cross-compilation
for arch in i386 armel armhf arm64 riscv64 powerpc ppc64el ia64; do
  dpkg --add-architecture "$arch"
done

if ! apt-get update -y; then
  echo "apt:update" >> "$FAIL_LOG"
fi

#— core build tools, formatters, analysis, science libs
for pkg in \
  build-essential gcc g++ clang lld llvm \
  clang-format clang-tidy uncrustify astyle editorconfig \
  make bmake mk-configure ninja-build cmake meson \
  autoconf automake libtool m4 gawk flex bison byacc \
  pkg-config file ca-certificates curl git unzip \
  libopenblas-dev liblapack-dev libeigen3-dev \
  strace ltrace linux-perf systemtap systemtap-sdt-dev crash \
  valgrind kcachegrind trace-cmd kernelshark \
  pre-commit configuredb \
  sqlite3 \
  libasan6 libubsan1 likwid hwloc; do
  apt_pin_install "$pkg"
done

#— Python & deep-learning / MLOps
for pkg in \
  python3 python3-pip python3-dev python3-venv python3-wheel \
  python3-numpy python3-scipy python3-pandas python3-yaml \
  python3-matplotlib python3-scikit-learn \
  python3-torch python3-torchvision python3-torchaudio \
  python3-onnx python3-onnxruntime; do
  apt_pin_install "$pkg"
done

for pip_pkg in \

  pre-commit cmake ninja meson configuredb compiledb pyyaml \

  tensorflow-cpu jax jaxlib \
  tensorflow-model-optimization mlflow onnxruntime-tools \
  pyyaml; do
  if ! pip3 install --no-cache-dir "$pip_pkg"; then
    echo "pip:$pip_pkg" >> "$FAIL_LOG"
  fi
done

# Ensure pre-commit is installed regardless of package source
if ! command -v pre-commit >/dev/null 2>&1; then
  echo "pre-commit not found, installing via pip..." >&2
  if ! pip3 install --no-cache-dir pre-commit; then
    echo "pip:pre-commit" >> "$FAIL_LOG"
    # Fallback to apt if available
    apt_pin_install pre-commit || true
  fi
fi

if command -v pre-commit >/dev/null 2>&1; then
  # ensure configuration exists
  if [ ! -f .pre-commit-config.yaml ]; then
    cat > .pre-commit-config.yaml <<'EOF'
minimum_pre_commit_version: '2.20.0'
repos:
  - repo: https://github.com/pre-commit/pre-commit-hooks
    rev: v4.6.0
    hooks:
      - id: trailing-whitespace
      - id: end-of-file-fixer
EOF
  fi
  pre-commit install --install-hooks >/dev/null 2>&1 || true
  pre-commit --version || true
fi

# Ensure configuredb is available
if ! command -v configuredb >/dev/null 2>&1; then
  # install local helper if package is missing
  ln -sf "$(pwd)/scripts/configuredb.sh" /usr/local/bin/configuredb
fi

if command -v configuredb >/dev/null 2>&1; then
  # initialize configuration and database
  configuredb >/dev/null 2>&1 || true
fi

# Provide a yacc alias when only bison or byacc are installed
if ! command -v yacc >/dev/null 2>&1; then
  if command -v byacc >/dev/null 2>&1; then
    ln -sf "$(command -v byacc)" /usr/local/bin/yacc
  elif command -v bison >/dev/null 2>&1; then
    ln -sf "$(command -v bison)" /usr/local/bin/yacc
  fi
fi

#— QEMU emulation for foreign binaries
for pkg in \
  qemu-user-static \
  qemu-system-x86 qemu-system-arm qemu-system-aarch64 \
  qemu-system-riscv64 qemu-system-ppc qemu-system-ppc64 qemu-utils; do
  apt_pin_install "$pkg"
done

#— multi-arch cross-compilers
for pkg in \
  bcc bin86 elks-libc \
  gcc-ia64-linux-gnu g++-ia64-linux-gnu \
  gcc-i686-linux-gnu g++-i686-linux-gnu \
  libc6-dev-i386 \
  gcc-aarch64-linux-gnu g++-aarch64-linux-gnu \
  gcc-arm-linux-gnueabi g++-arm-linux-gnueabi \
  gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf \
  gcc-riscv64-linux-gnu g++-riscv64-linux-gnu \
  gcc-powerpc-linux-gnu g++-powerpc-linux-gnu \
  gcc-powerpc64-linux-gnu g++-powerpc64-linux-gnu \
  gcc-powerpc64le-linux-gnu g++-powerpc64le-linux-gnu \
  libc6-dev-powerpc-cross libc6-dev-ppc64-cross \
  gcc-m68k-linux-gnu g++-m68k-linux-gnu \
  gcc-hppa-linux-gnu g++-hppa-linux-gnu \
  gcc-loongarch64-linux-gnu g++-loongarch64-linux-gnu \
  gcc-mips-linux-gnu g++-mips-linux-gnu \
  gcc-mipsel-linux-gnu g++-mipsel-linux-gnu \
  gcc-mips64-linux-gnuabi64 g++-mips64-linux-gnuabi64 \
  gcc-mips64el-linux-gnuabi64 g++-mips64el-linux-gnuabi64; do
  apt_pin_install "$pkg"
done

#— high-level language runtimes and tools
for pkg in \
  golang-go nodejs npm typescript \
  rustc cargo clippy rustfmt \
  lua5.4 liblua5.4-dev luarocks \
  ghc cabal-install hlint stylish-haskell \
  sbcl ecl clisp cl-quicklisp slime cl-asdf \
  ldc gdc dmd-compiler dub libphobos-dev \
  chicken-bin libchicken-dev chicken-doc \
  openjdk-17-jdk maven gradle dotnet-sdk-8 mono-complete \
  swift swift-lldb swiftpm kotlin gradle-plugin-kotlin \
  ruby ruby-dev gem bundler php-cli php-dev composer phpunit \
  r-base r-base-dev dart flutter gnat gprbuild gfortran gnucobol \
  fpc lazarus zig nim nimble crystal shards gforth; do
  apt_pin_install "$pkg"
done

# Verify swift installation and provide guidance if missing
if ! command -v swift >/dev/null 2>&1; then
  echo "Swift not found after installation. Attempting to install via apt..."
  apt_pin_install swift || true
  apt_pin_install swift-lldb || true
  apt_pin_install swiftpm || true
fi

#— GUI & desktop-dev frameworks
for pkg in \
  libqt5-dev qtcreator libqt6-dev \
  libgtk1.2-dev libgtk2.0-dev libgtk-3-dev libgtk-4-dev \
  libfltk1.3-dev xorg-dev libx11-dev libxext-dev \
  libmotif-dev openmotif cde \
  xfce4-dev-tools libxfce4ui-2-dev lxde-core lxqt-dev-tools \
  libefl-dev libeina-dev \
  libwxgtk3.0-dev libwxgtk3.0-gtk3-dev \
  libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev \
  libglfw3-dev libglew-dev; do
  apt_pin_install "$pkg"
done

#— containers, virtualization, HPC, debug
for pkg in \
  docker.io podman buildah virt-manager libvirt-daemon-system qemu-kvm \
  gdb lldb perf gcovr lcov bcc-tools bpftrace \
  openmpi-bin libopenmpi-dev mpich; do
  apt_pin_install "$pkg"
done

#— IA-16 (8086/286) cross-compiler
IA16_VER=$(curl -fsSL https://api.github.com/repos/tkchia/gcc-ia16/releases/latest \
           | awk -F\" '/tag_name/{print $4; exit}')
if ! curl -fsSL "https://github.com/tkchia/gcc-ia16/releases/download/${IA16_VER}/ia16-elf-gcc-linux64.tar.xz" \
  | tar -Jx -C /opt; then
  echo "ia16-gcc" >> "$FAIL_LOG"
fi
echo 'export PATH=/opt/ia16-elf-gcc/bin:$PATH' > /etc/profile.d/ia16.sh
export PATH=/opt/ia16-elf-gcc/bin:$PATH

#— protoc installer (pinned)
PROTO_VERSION=25.1
if curl -fsSL "https://raw.githubusercontent.com/protocolbuffers/protobuf/v${PROTO_VERSION}/protoc-${PROTO_VERSION}-linux-x86_64.zip" \
  -o /tmp/protoc.zip; then
  unzip -d /usr/local /tmp/protoc.zip || echo "protoc:unzip" >> "$FAIL_LOG"
  rm /tmp/protoc.zip
else
  echo "protoc" >> "$FAIL_LOG"
fi

#— gmake alias
command -v gmake >/dev/null 2>&1 || ln -s "$(command -v make)" /usr/local/bin/gmake

# Install git hooks and sanity check dev tools
clang-format --version >/dev/null 2>&1 || true
clang-tidy --version >/dev/null 2>&1 || true

# Generate a compilation database for clang tooling
if ! cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON >/dev/null 2>&1; then
  echo "cmake:compdb" >> "$FAIL_LOG"
fi
if [ -f build/compile_commands.json ]; then
  ln -sf build/compile_commands.json compile_commands.json
elif command -v compiledb >/dev/null 2>&1; then
  echo "Running compiledb as fallback" >&2
  if [ -f Makefile ]; then
    compiledb -n make >/dev/null 2>&1 || true
  elif [ -f build.ninja ]; then
    compiledb -n ninja >/dev/null 2>&1 || true
  fi
fi

# verify links from the README
if [ -f README.md ]; then
  echo "Checking repository links..."
  bad_links=""
  while read -r url; do
    status=$(curl -fsL -o /dev/null -w '%{http_code}' --connect-timeout 10 "$url" || echo "000")
    if [ "$status" != "200" ]; then
      echo "Link failure: $url -> $status"
      bad_links+="$url\n"
    fi
  done < <(grep -oE 'https?://[^ )]+"?' README.md | tr -d '"')
  if [ -n "$bad_links" ]; then
    printf '%b' "$bad_links" > linkcheck.log
    echo "Nonfunctional links logged to linkcheck.log"
    echo "linkcheck" >> "$FAIL_LOG"
  else
    echo "All links OK"
  fi
fi

# Clone OpenMach headers if not already present
if [ ! -d openmach ]; then
  echo "Cloning OpenMach repository..." >&2
  if ! git clone --depth=1 https://github.com/mach4/mach4.git openmach; then
    echo "openmach" >> "$FAIL_LOG"
  fi
fi

#— clean up
apt-get clean
rm -rf /var/lib/apt/lists/*

if [ -s "$FAIL_LOG" ]; then
  echo "Some installations failed. See $FAIL_LOG for details." >&2
  cat "$FAIL_LOG" >&2
else
  echo "All packages installed successfully" >&2
fi

exit 0
