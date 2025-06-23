#!/usr/bin/env bash
#===============================================================================
# Title:   environment-setup.sh
# Purpose: Install and configure all required packages for development.
# Usage:   ./environment-setup.sh [--offline] [--help]
# Options:
#   --offline   Install from local .deb packages (offline mode)
#   --help      Show this help message and exit
#===============================================================================
set -Eeuo pipefail
trap 'log_failure "Failure at line $LINENO: $BASH_COMMAND"' ERR

#-------------------------------------------------------------------------------
# Globals
#-------------------------------------------------------------------------------
LOG="/tmp/setup.log"
FAIL_LOG="/tmp/setup_failures.log"
OFFLINE=false
OFFLINE_DIR="offline_packages"
CLANG_VERSION=18
# initialise default compiler flags to avoid unbound variable errors when
# appending optimisation options later on.
CFLAGS=""
CXXFLAGS=""

#-------------------------------------------------------------------------------
# Logging functions
#-------------------------------------------------------------------------------
log_info() { echo "[INFO]    $*" | tee -a "$LOG"; }
log_error() { echo "[ERROR]   $*" | tee -a "$FAIL_LOG" >&2; }
log_failure() {
    echo "[FATAL]   $*" | tee -a "$FAIL_LOG" >&2
    exit 1
}

# Redirect all stdout/stderr to log
exec > >(tee -a "$LOG") 2>&1
set -x

export DEBIAN_FRONTEND=noninteractive

#-------------------------------------------------------------------------------
# Usage
#-------------------------------------------------------------------------------
usage() {
    cat <<EOF
Usage: $(basename "$0") [--offline] [--help]

Options:
  --offline   Use local .deb packages under '$OFFLINE_DIR'
  --help      Display this help and exit
EOF
    exit 0
}

#-------------------------------------------------------------------------------
# Parse options
#-------------------------------------------------------------------------------
while (($#)); do
    case "$1" in
    --offline)
        OFFLINE=true
        shift
        ;;
    --help) usage ;;
    *)
        log_error "Unknown option: $1"
        usage
        ;;
    esac
done

#-------------------------------------------------------------------------------
# Apt update & upgrade
#-------------------------------------------------------------------------------
if ! apt-get update -y; then
    log_error "apt-get update failed"
fi
if ! apt-get dist-upgrade -y; then
    log_error "apt-get dist-upgrade failed"
fi

#-------------------------------------------------------------------------------
# Clone or install OpenMach
#-------------------------------------------------------------------------------
OPENMACH_REPO=${OPENMACH_REPO:-https://github.com/machkernel/openmach.git}
if $OFFLINE; then
    log_info "Offline mode enabled"
    if compgen -G "${OFFLINE_DIR}/*.deb" >/dev/null; then
        dpkg -i "${OFFLINE_DIR}"/*.deb || log_error "dpkg install failed"
    else
        log_info "No .deb files found in '${OFFLINE_DIR}'"
    fi
else
    if [[ ! -d openmach ]]; then
        if GIT_TERMINAL_PROMPT=0 git ls-remote "$OPENMACH_REPO" HEAD >/dev/null 2>&1; then
            if git clone "$OPENMACH_REPO" openmach; then
                log_info "Cloned OpenMach from $OPENMACH_REPO"
            else
                log_error "Failed to clone OpenMach"
            fi
        else
            log_error "OpenMach repository unreachable, skipping clone"
        fi
    else
        log_info "OpenMach directory already exists"
    fi
fi

#-------------------------------------------------------------------------------
# install_pkg: try apt, pip, npm, then custom handlers
#-------------------------------------------------------------------------------
##
# \brief Install a package using various backends.
#
# The function first tries apt, then pip, npm, and finally custom handlers for
# specific packages.
#
# \param pkg Name of the package to install.
# \return Zero on success, non-zero on failure.
install_pkg() {
    local pkg="$1"
    log_info "Installing package: $pkg"

    if apt-get install -y "$pkg"; then
        log_info "$pkg installed via apt"
        [[ $pkg == "shellcheck" ]] && configure_shellcheck
        return 0
    fi
    log_error "apt-get failed for $pkg"

    if pip install "$pkg"; then
        log_info "$pkg installed via pip"
        return 0
    fi
    log_error "pip failed for $pkg"

    if npm install -g "$pkg"; then
        log_info "$pkg installed via npm"
        return 0
    fi
    log_error "npm failed for $pkg"

    case "$pkg" in
    shellcheck)
        install_shellcheck
        return 0
        ;;
    capnproto)
        wget -qO- \
            https://github.com/capnproto/capnproto/archive/refs/tags/v0.9.1.tar.gz |
            tar xz
        pushd capnproto-0.9.1 >/dev/null
        ./setup-makefiles.sh && ./configure && make -j"$(nproc)" && make install
        popd >/dev/null
        log_info "capnproto built from source"
        return 0
        ;;
    isabelle)
        wget -qO- \
            https://isabelle.in.tum.de/dist/Isabelle2023-1_linux.tar.gz | tar xz
        install -d /opt/isabelle
        mv Isabelle2023-1 /opt/isabelle
        ln -sf /opt/isabelle/Isabelle2023-1/bin/isabelle /usr/local/bin/isabelle
        log_info "isabelle installed from archive"
        return 0
        ;;
    *)
        log_error "No handler for package '$pkg'"
        return 1
        ;;
    esac
}

##
# \brief Configure default options for shellcheck.
#
# Creates a system-wide configuration file enabling the GCC output format and
# ignoring common includes.
#
# \return Zero on success.
configure_shellcheck() {
    local rc=/etc/shellcheckrc
    cat <<'EOF' >"$rc"
format=gcc
exclude=SC1091
EOF
    log_info "shellcheck configuration created at $rc"
}

##
# \brief Install shellcheck from its official binary release.
#
# Downloads the release tarball, installs the binary to /usr/local/bin and then
# runs \ref configure_shellcheck.
#
# \return Zero on success, non-zero on failure.
install_shellcheck() {
    wget -qO- \
        https://github.com/koalaman/shellcheck/releases/download/v0.9.0/shellcheck-v0.9.0.linux.x86_64.tar.xz |
        tar xJ &&
        install -m 0755 shellcheck-v0.9.0/shellcheck /usr/local/bin/shellcheck &&
        rm -rf shellcheck-v0.9.0 &&
        configure_shellcheck &&
        log_info "shellcheck installed from binary"
}

##
# \brief Determine whether an apt package exists in the repositories.
#
# This helper checks the package cache to avoid attempting installation of
# non-existent packages. Skipping such packages keeps the log free from
# unnecessary apt-get failure messages.
#
# \param pkg Package name to query.
# \return Zero if the package is found, non-zero otherwise.
package_available() {
    local pkg="$1"
    apt-cache show "$pkg" >/dev/null 2>&1
}

##
# \brief Determine the newest installed Clang version.
#
# Starting from version 20 the function searches the package
# repositories for descending releases and selects the first
# available option.  It defaults to version 18 but will fall
# back as far as clang-11 to support older distributions.
#
# The detected version is stored in the global \c CLANG_VERSION
# variable so subsequent package installation and environment
# exports remain consistent.
select_clang_version() {
    local candidates=(20 18 14 11)
    for ver in "${candidates[@]}"; do
        if package_available "clang-${ver}"; then
            CLANG_VERSION=$ver
            return 0
        fi
    done
    log_error "No supported clang version found"
}

select_clang_version

##
# \brief Configure 80386-specific Clang optimization flags.
#
# Sets \c CFLAGS and \c CXXFLAGS with a conservative yet optimized set of
# parameters tailored for a 80386 processor. These options restrict the
# instruction set to i386, disable unsupported extensions and enable
# alignment tweaks suited for the 16-byte prefetch queue.
#
# The resulting flags offer cycle-level improvements on vintage hardware while
# remaining compatible with modern toolchains.
configure_clang_386_flags() {
    local opt_flags=(
        -march=i386 -mtune=i386 -m32
        -mno-sse -mno-sse2 -mno-sse3 -mno-ssse3
        -mno-sse4 -mno-sse4.1 -mno-sse4.2
        -mno-avx -mno-avx2 -mno-avx512f
        -mno-mmx -mno-3dnow
        -mno-cx8 -mno-cx16 -mno-sahf -mno-movbe
        -mno-aes -mno-sha -mno-pclmul -mno-popcnt -mno-abm
        -mno-lwp -mno-fma -mno-fma4 -mno-xop
        -mno-bmi -mno-bmi2 -mno-tbm -mno-lzcnt
        -mno-rtm -mno-hle -mno-rdrnd -mno-f16c
        -mno-fsgsbase -mno-rdseed -mno-prfchw -mno-adx
        -mno-fxsr -mno-xsave -mno-xsaveopt
        -fno-builtin-bswap16 -fno-builtin-bswap32 -fno-builtin-bswap64
        -mfpmath=387 -m80387 -mhard-float -fexcess-precision=standard
        -ffp-contract=off
        -malign-data=compat -malign-functions=16 -malign-jumps=16
        -malign-loops=16 -mstack-alignment=4
        -mstringop-strategy=libcall
        -O3 -fomit-frame-pointer -fstrict-aliasing -fno-semantic-interposition
        -fmerge-all-constants -fno-common -ffunction-sections -fdata-sections
        -falign-functions=16 -falign-loops=16
        -foptimize-sibling-calls -findirect-inlining -finline-limit=1000
        -frename-registers -fweb -fira-algorithm=CB -fira-hoist-pressure
        -fsched-pressure -fsched-spec-load -fmodulo-sched
        -fmodulo-sched-allow-regmoves
        -fprofile-use=/tmp/pgo-386 -fauto-profile=/tmp/386-profile.afdo
        -mcmodel=small -mplt -mno-tls-direct-seg-refs
    )
    export CFLAGS="${CFLAGS} ${opt_flags[*]}"
    export CXXFLAGS="$CFLAGS"
}

configure_clang_386_flags

##
# \brief Create wrapper symlinks so cc and c++ use clang.
#
# The function places symlinks in /usr/local/bin pointing to the
# versioned clang binaries selected earlier. This ensures that build
# systems relying on the generic compiler names invoke clang by default.
#
# \return 0 on success.
create_clang_shims() {
    local clang_bin="/usr/bin/clang-${CLANG_VERSION}"
    local clangpp_bin="/usr/bin/clang++-${CLANG_VERSION}"
    if [[ -x $clang_bin && -x $clangpp_bin ]]; then
        install -d /usr/local/bin
        ln -sf "$clang_bin" /usr/local/bin/cc
        ln -sf "$clangpp_bin" /usr/local/bin/c++
        log_info "Clang shims installed for cc and c++"
    else
        log_error "Expected clang binaries not found: ${CLANG_VERSION}"
    fi
}


#-------------------------------------------------------------------------------
# Package list
#-------------------------------------------------------------------------------
packages=(

    build-essential git wget curl
    clang-${CLANG_VERSION} lld-${CLANG_VERSION} \
    llvm-${CLANG_VERSION}-dev libclang-${CLANG_VERSION}-dev polly
    clang-tools-${CLANG_VERSION} clang-tidy-${CLANG_VERSION} \
    clang-format-${CLANG_VERSION} clangd-${CLANG_VERSION}
    ccache lldb gdb bolt llvm-bolt
    cmake make ninja-build meson
    doxygen graphviz python3-sphinx python3-breathe
    shellcheck yamllint
    python3 python3-pip python3-venv python3-setuptools python3-wheel
    bison byacc
    nodejs npm yarnpkg
    coq coqide tla4tools isabelle
    afl++ honggfuzz cargo-fuzz
    qemu-system-x86 qemu-utils valgrind lcov gcovr
    tmux cloc cscope libperl-dev gdb-multiarch lizard
    gcc-aarch64-linux-gnu gcc-arm-linux-gnueabihf \
    gcc-riscv64-linux-gnu gcc-powerpc-linux-gnu
) # cross toolchains

# gcc-multilib conflicts with the cross packages above, so it is omitted.

##
# \brief Install pre-commit when missing and set up hooks.
#
# The function ensures the `pre-commit` executable is available. When not
# installed system-wide, it uses `pip` to install the tool for the current
# user. If a `.pre-commit-config.yaml` file exists in the repository root,
# the hooks are installed automatically.
install_precommit() {
    if ! command -v pre-commit >/dev/null 2>&1; then
        if pip install pre-commit; then
            log_info "pre-commit installed via pip"
        else
            log_error "failed to install pre-commit"
        fi
    fi

    if [[ -f .pre-commit-config.yaml ]]; then
        if pre-commit install; then
            log_info "pre-commit hooks installed"
        else
            log_error "failed to install pre-commit hooks"
        fi
    fi
}

#-------------------------------------------------------------------------------
# Install all packages
#-------------------------------------------------------------------------------
for pkg in "${packages[@]}"; do
    if package_available "$pkg"; then
        install_pkg "$pkg" || true
    else
        log_error "Skipping unknown package '$pkg'"
    fi
done

#-------------------------------------------------------------------------------
# Summarize & auto-repair failures
#-------------------------------------------------------------------------------
if [[ -s "$FAIL_LOG" ]]; then
    log_info "Packages failed to install:"
    sed 's/^/[FAIL] /' "$FAIL_LOG" | tee -a "$LOG"
    log_info "Attempting automatic repair (apt-get -f install)"
    if apt-get -f install -y >>"$LOG" 2>&1; then
        : >"$FAIL_LOG"
        log_info "Automatic repair succeeded"
    else
        log_error "Automatic repair failed; inspect $FAIL_LOG"
    fi
else
    log_info "All packages installed successfully"
fi

install_precommit

create_clang_shims

##
# \brief Generate a compile_commands.json database when absent.
#
# The database improves IDE integration and allows clang tooling to
# operate without invoking the build system.  The build directory is
# created if necessary and configured with CMake's
# `CMAKE_EXPORT_COMPILE_COMMANDS` option.
generate_compile_commands() {
    if [[ ! -f build/compile_commands.json ]]; then
        cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
            "${LITES_SRC_DIR:+-DLITES_SRC_DIR=$LITES_SRC_DIR}" &&
            log_info "compile_commands.json generated"
    fi
}

#-------------------------------------------------------------------------------
# Environment exports
#-------------------------------------------------------------------------------
export CC="ccache clang-${CLANG_VERSION}"
export CXX="ccache clang++-${CLANG_VERSION}"
export CLANG_TIDY="clang-tidy-${CLANG_VERSION}"
export PATH="/usr/lib/ccache:$PATH"
export CFLAGS="${CFLAGS} -Wall -Wextra -Werror"
export CXXFLAGS="${CXXFLAGS:-$CFLAGS}"
# Harden binaries by disallowing executable stacks
# Harden binaries by disallowing executable stacks. Add the flag to
# existing options if needed.
export LDFLAGS="-fuse-ld=lld -flto ${LDFLAGS:+$LDFLAGS }-Wl,-z,noexecstack"
export LLVM_PROFILE_FILE="/tmp/profiles/default.profraw"
export CLANG_EXTRA_FLAGS="-mllvm -polly -mllvm -polly-opt-max-coefficient=32 \
    -mllvm -polly-pattern-matching-based-opts=true -fgraphite-identity \
    -fgraphite"

generate_compile_commands

log_info "Environment setup complete."
