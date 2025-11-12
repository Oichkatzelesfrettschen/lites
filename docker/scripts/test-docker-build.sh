#!/usr/bin/env bash
set -euo pipefail

##
# @file test-docker-build.sh
# @brief Test script to verify Docker environment and build
#
# This script validates that the Docker development environment is properly
# configured and can successfully build Lites for i386.
##

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

##
# @brief Print colored status message
##
status() {
    echo -e "${GREEN}[TEST]${NC} $*"
}

##
# @brief Print error message
##
error() {
    echo -e "${RED}[ERROR]${NC} $*" >&2
}

##
# @brief Print warning message
##
warning() {
    echo -e "${YELLOW}[WARN]${NC} $*"
}

##
# @brief Check if a command exists
##
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

##
# @brief Run a test and report status
##
run_test() {
    local test_name="$1"
    shift
    
    status "Running: $test_name"
    
    if "$@"; then
        echo -e "  ${GREEN}✓${NC} $test_name passed"
        return 0
    else
        echo -e "  ${RED}✗${NC} $test_name failed"
        return 1
    fi
}

##
# @brief Test Docker installation
##
test_docker_installed() {
    command_exists docker
}

##
# @brief Test Docker Compose installation
##
test_docker_compose_installed() {
    docker compose version >/dev/null 2>&1 || command_exists docker-compose
}

##
# @brief Test Docker daemon is running
##
test_docker_running() {
    docker info >/dev/null 2>&1
}

##
# @brief Test Dockerfile exists
##
test_dockerfile_exists() {
    [[ -f "${PROJECT_ROOT}/docker/Dockerfile.i386-dev" ]]
}

##
# @brief Test docker-compose.yml exists
##
test_compose_file_exists() {
    [[ -f "${PROJECT_ROOT}/docker/docker-compose.yml" ]]
}

##
# @brief Test scripts exist and are executable
##
test_scripts_exist() {
    local scripts=(
        "docker/scripts/build-lites-i386.sh"
        "docker/scripts/run-qemu-i386.sh"
        "docker/scripts/docker-shell.sh"
    )
    
    for script in "${scripts[@]}"; do
        local full_path="${PROJECT_ROOT}/${script}"
        if [[ ! -f "$full_path" ]]; then
            error "Script not found: $script"
            return 1
        fi
        if [[ ! -x "$full_path" ]]; then
            error "Script not executable: $script"
            return 1
        fi
    done
    
    return 0
}

##
# @brief Test Docker image can be built
##
test_docker_build() {
    cd "${PROJECT_ROOT}"
    docker compose -f docker/docker-compose.yml build --quiet lites-i386-dev >/dev/null 2>&1
}

##
# @brief Test container can start
##
test_container_start() {
    cd "${PROJECT_ROOT}"
    docker compose -f docker/docker-compose.yml run --rm lites-i386-dev echo "Container started successfully" >/dev/null 2>&1
}

##
# @brief Test MIG is available in container
##
test_mig_available() {
    cd "${PROJECT_ROOT}"
    docker compose -f docker/docker-compose.yml run --rm lites-i386-dev which mig >/dev/null 2>&1
}

##
# @brief Test GCC multilib is available
##
test_gcc_multilib() {
    cd "${PROJECT_ROOT}"
    docker compose -f docker/docker-compose.yml run --rm lites-i386-dev \
        bash -c "echo 'int main() { return 0; }' | gcc -m32 -x c - -o /tmp/test.out" >/dev/null 2>&1
}

##
# @brief Test QEMU is available
##
test_qemu_available() {
    cd "${PROJECT_ROOT}"
    docker compose -f docker/docker-compose.yml run --rm lites-i386-dev which qemu-system-i386 >/dev/null 2>&1
}

##
# @brief Main test runner
##
main() {
    local failed=0
    local total=0
    
    echo "======================================"
    echo "Lites i386 Docker Environment Tests"
    echo "======================================"
    echo ""
    
    status "Testing prerequisites..."
    
    # Basic tests
    run_test "Docker installed" test_docker_installed || ((failed++))
    ((total++))
    
    run_test "Docker Compose installed" test_docker_compose_installed || ((failed++))
    ((total++))
    
    run_test "Docker daemon running" test_docker_running || ((failed++))
    ((total++))
    
    echo ""
    status "Testing repository structure..."
    
    run_test "Dockerfile exists" test_dockerfile_exists || ((failed++))
    ((total++))
    
    run_test "docker-compose.yml exists" test_compose_file_exists || ((failed++))
    ((total++))
    
    run_test "Scripts exist and executable" test_scripts_exist || ((failed++))
    ((total++))
    
    echo ""
    status "Testing Docker build (this may take a while)..."
    
    run_test "Docker image builds" test_docker_build || ((failed++))
    ((total++))
    
    echo ""
    status "Testing container functionality..."
    
    run_test "Container starts" test_container_start || ((failed++))
    ((total++))
    
    run_test "MIG available" test_mig_available || ((failed++))
    ((total++))
    
    run_test "GCC multilib works" test_gcc_multilib || ((failed++))
    ((total++))
    
    run_test "QEMU available" test_qemu_available || ((failed++))
    ((total++))
    
    echo ""
    echo "======================================"
    echo "Test Results"
    echo "======================================"
    echo "Total tests: $total"
    echo -e "Passed: ${GREEN}$((total - failed))${NC}"
    if [[ $failed -gt 0 ]]; then
        echo -e "Failed: ${RED}${failed}${NC}"
    else
        echo -e "Failed: ${failed}"
    fi
    
    if [[ $failed -eq 0 ]]; then
        echo ""
        echo -e "${GREEN}✓ All tests passed!${NC}"
        echo ""
        echo "You can now:"
        echo "  1. Build Lites: make -f Makefile.docker docker-build-lites"
        echo "  2. Run in QEMU: make -f Makefile.docker docker-run-qemu"
        echo "  3. Enter shell: make -f Makefile.docker docker-shell"
        return 0
    else
        echo ""
        echo -e "${RED}✗ Some tests failed${NC}"
        echo ""
        echo "Please fix the issues above before proceeding."
        return 1
    fi
}

main "$@"
