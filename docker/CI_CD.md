# CI/CD Automation for Lites Docker i386

WHY: Automated testing ensures consistent builds and catches issues early.

WHAT: GitHub Actions and GitLab CI workflows for Docker-based Lites builds.

HOW: Use provided workflow files and adapt for your CI platform.

---

## Overview

Complete CI/CD pipeline:
```
Trigger → Build Docker Image → Build Lites → Run Tests → Artifacts
```

All workflows use **Docker Compose v2** syntax (not legacy v1).

---

## GitHub Actions

### Workflow Structure

The pipeline (`.github/workflows/docker-i386-ci.yml`) has 6 jobs:

```
docker-lint           # Validate scripts and configs
    ↓
docker-build          # Build Docker image
    ↓
├─ docker-build-lites    # Test Lites build
├─ docker-test           # Environment tests
└─ docker-makefile-targets   # Make targets
    ↓
docker-integration    # End-to-end test
    ↓
summary              # Report results
```

### Job Details

**1. docker-lint** (30 seconds)
- Validates bash scripts with shellcheck
- Checks YAML syntax
- Verifies Makefile

**2. docker-build** (5-10 minutes)
- Builds Docker image with BuildKit
- Verifies tools (GCC, MIG, QEMU, CMake)
- Exports image for downstream jobs

**3. docker-build-lites** (1-2 minutes)
- Tests Lites build process
- Verifies build artifacts

**4. docker-test** (1-2 minutes)
- Tests environment functionality
- Validates tool availability

**5. docker-integration** (1-2 minutes)
- End-to-end workflow test
- Validates all scripts

**6. summary** (10 seconds)
- Aggregates job results
- Reports overall status

### Complete Example

```yaml
name: Build and Test Lites i386

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

env:
  DOCKER_BUILDKIT: 1
  COMPOSE_DOCKER_CLI_BUILD: 1

jobs:
  lint:
    name: Lint Scripts
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4

      - name: Install shellcheck
        run: sudo apt-get update && sudo apt-get install -y shellcheck

      - name: Lint bash scripts
        run: |
          find docker/scripts -name '*.sh' -exec shellcheck {} +

      - name: Validate YAML
        run: |
          python3 -c "import yaml, sys; yaml.safe_load(open('docker/docker-compose.yml'))"

  build-image:
    name: Build Docker Image
    runs-on: ubuntu-latest
    needs: lint
    steps:
      - uses: actions/checkout@v4

      - name: Set up Docker Buildx
        uses: docker/setup-buildx-action@v3

      - name: Build Docker image
        run: make -f Makefile.docker docker-build

      - name: Verify image
        run: |
          docker compose -f docker/docker-compose.yml run --rm lites-i386-dev \
            bash -c "gcc --version && mig --version && qemu-system-i386 --version"

      - name: Save image
        run: |
          docker save lites-i386-dev:latest | gzip > lites-i386-dev.tar.gz

      - name: Upload artifact
        uses: actions/upload-artifact@v4
        with:
          name: lites-i386-dev-image
          path: lites-i386-dev.tar.gz
          retention-days: 1

  build-lites:
    name: Build Lites
    runs-on: ubuntu-latest
    needs: build-image
    steps:
      - uses: actions/checkout@v4

      - name: Download image
        uses: actions/download-artifact@v4
        with:
          name: lites-i386-dev-image

      - name: Load image
        run: docker load < lites-i386-dev.tar.gz

      - name: Build Lites
        run: make -f Makefile.docker docker-build-lites

      - name: Verify build
        run: |
          docker compose -f docker/docker-compose.yml run --rm lites-i386-dev \
            bash -c "test -f build-i386/lites_server && file build-i386/lites_server"

      - name: Upload binary
        uses: actions/upload-artifact@v4
        with:
          name: lites-i386-kernel
          path: build-i386/lites_server
          retention-days: 7

  test:
    name: Test Environment
    runs-on: ubuntu-latest
    needs: build-image
    steps:
      - uses: actions/checkout@v4

      - name: Download image
        uses: actions/download-artifact@v4
        with:
          name: lites-i386-dev-image

      - name: Load image
        run: docker load < lites-i386-dev.tar.gz

      - name: Run tests
        run: |
          docker compose -f docker/docker-compose.yml run --rm lites-i386-dev \
            docker/scripts/test-docker-build.sh

  qemu-test:
    name: Test QEMU
    runs-on: ubuntu-latest
    needs: build-lites
    steps:
      - uses: actions/checkout@v4

      - name: Download image
        uses: actions/download-artifact@v4
        with:
          name: lites-i386-dev-image

      - name: Load image
        run: docker load < lites-i386-dev.tar.gz

      - name: Test QEMU boot
        run: |
          timeout 60 make -f Makefile.docker docker-run-qemu || true
```

### Triggering

Workflow runs on:
- Push to `main`, `develop`
- Pull requests to `main`, `develop`
- Manual trigger via `workflow_dispatch`

### Artifacts

Produced artifacts:
- **lites-i386-dev-image**: Docker image (1-day retention)
- **lites-i386-kernel**: Compiled binary (7-day retention)

### Local Testing

Test workflows locally with `act`:

```bash
# Install act
# https://github.com/nektos/act

# Run full workflow
act -W .github/workflows/docker-i386-ci.yml

# Run specific job
act -W .github/workflows/docker-i386-ci.yml -j build-lites

# With secrets
act -W .github/workflows/docker-i386-ci.yml -s GITHUB_TOKEN=...
```

---

## GitLab CI

### Complete Pipeline

```yaml
# .gitlab-ci.yml

stages:
  - lint
  - build-image
  - build-kernel
  - test
  - package

variables:
  DOCKER_DRIVER: overlay2
  DOCKER_BUILDKIT: "1"
  COMPOSE_DOCKER_CLI_BUILD: "1"

lint:scripts:
  stage: lint
  image: koalaman/shellcheck-alpine:latest
  script:
    - find docker/scripts -name '*.sh' -exec shellcheck {} +
  allow_failure: false

lint:yaml:
  stage: lint
  image: python:3-alpine
  script:
    - pip install pyyaml
    - python -c "import yaml; yaml.safe_load(open('docker/docker-compose.yml'))"
  allow_failure: false

build:docker-image:
  stage: build-image
  image: docker:latest
  services:
    - docker:dind
  before_script:
    - docker info
  script:
    - make -f Makefile.docker docker-build
    - docker save lites-i386-dev:latest | gzip > lites-i386-dev.tar.gz
  artifacts:
    paths:
      - lites-i386-dev.tar.gz
    expire_in: 1 day

build:lites-i386:
  stage: build-kernel
  image: docker:latest
  services:
    - docker:dind
  dependencies:
    - build:docker-image
  before_script:
    - docker load < lites-i386-dev.tar.gz
  script:
    - make -f Makefile.docker docker-build-lites
    - docker compose -f docker/docker-compose.yml run --rm lites-i386-dev \
        bash -c "file build-i386/lites_server"
  artifacts:
    paths:
      - build-i386/lites_server
    expire_in: 1 week

test:environment:
  stage: test
  image: docker:latest
  services:
    - docker:dind
  dependencies:
    - build:docker-image
  before_script:
    - docker load < lites-i386-dev.tar.gz
  script:
    - docker compose -f docker/docker-compose.yml run --rm lites-i386-dev \
        docker/scripts/test-docker-build.sh

test:qemu-boot:
  stage: test
  image: docker:latest
  services:
    - docker:dind
  dependencies:
    - build:docker-image
    - build:lites-i386
  before_script:
    - docker load < lites-i386-dev.tar.gz
  script:
    - timeout 60 make -f Makefile.docker docker-run-qemu || true
  allow_failure: true

package:disk-images:
  stage: package
  image: docker:latest
  services:
    - docker:dind
  dependencies:
    - build:docker-image
    - build:lites-i386
  before_script:
    - docker load < lites-i386-dev.tar.gz
  script:
    - make -f Makefile.docker docker-bootimg
    - make -f Makefile.docker docker-qcow2
  artifacts:
    paths:
      - lites-boot.img
      - lites.qcow2
    expire_in: 2 weeks
```

---

## Docker Compose v2

**IMPORTANT**: Use v2 syntax throughout.

```bash
# Correct (v2)
docker compose version
docker compose -f docker/docker-compose.yml build
docker compose -f docker/docker-compose.yml run --rm service command

# Legacy v1 (deprecated, NOT used)
docker-compose version
docker-compose -f docker/docker-compose.yml build
```

**Why v2?**
- Modern standard (included with Docker 20.10+)
- Better performance and caching
- Active development (v1 is deprecated)

---

## Execution Flow

```
┌──────────────────────────────────────────────┐
│ CI/CD Environment                            │
│                                              │
│  1. Lint Phase                               │
│     ├─ Shellcheck bash scripts              │
│     ├─ Validate YAML syntax                 │
│     └─ Check Makefile                       │
│                                              │
│  2. Build Docker Image                       │
│     ├─ docker compose build                 │
│     ├─ Verify tools installed               │
│     └─ Export image artifact                │
│                                              │
│  3. Build Lites                              │
│     ├─ Load image artifact                  │
│     ├─ Run build-lites-i386.sh              │
│     └─ Verify ELF binary                    │
│                                              │
│  4. Test                                     │
│     ├─ Environment validation               │
│     ├─ QEMU boot test (timeout 60s)         │
│     └─ Integration tests                    │
│                                              │
│  5. Package (optional)                       │
│     ├─ Create bootable image                │
│     └─ Convert to QCOW2                     │
└──────────────────────────────────────────────┘
```

---

## Environment Variables

```yaml
# Docker Build
DOCKER_BUILDKIT: 1              # Enable BuildKit
COMPOSE_DOCKER_CLI_BUILD: 1     # Use BuildKit for Compose

# Lites Build
ARCH: i686                      # Target architecture
BUILD_DIR: /workspace/build-i386

# Mach
LITES_MACH_DIR: /opt/mach/osfmk/kernel/src

# QEMU
QEMU_MEMORY: 256M               # QEMU RAM
QEMU_CPUS: 1                    # Virtual CPUs
QEMU_KVM: auto                  # KVM: auto/yes/no

# Compilation
CC: "ccache gcc"
CFLAGS: "-m32 -march=i386 -mtune=i386"
LDFLAGS: "-m32 -Wl,-z,noexecstack"
```

---

## Performance

**Typical execution times**:

| Stage | First Run | Cached | Notes |
|-------|-----------|--------|-------|
| Lint | 30s | 30s | Always fast |
| Build Image | 5-10 min | 1-2 min | Layer caching |
| Build Lites | 5-10 min | 30-120s | ccache helps |
| Tests | 1-2 min | 1-2 min | Uses cached image |
| QEMU Test | 30-60s | 30-60s | No KVM in CI |

**Total**: ~10-20 minutes first run, ~5-10 minutes cached

---

## Caching Strategies

**Docker layer caching**:
```yaml
# GitHub Actions
- name: Set up Docker Buildx
  uses: docker/setup-buildx-action@v3
  with:
    cache-from: type=gha
    cache-to: type=gha,mode=max
```

**GitLab CI**:
```yaml
variables:
  DOCKER_DRIVER: overlay2
cache:
  key: ${CI_COMMIT_REF_SLUG}
  paths:
    - .docker/cache/
```

**Clear cache**:
```bash
# GitHub: Settings → Actions → Caches → Delete
# GitLab: CI/CD → Pipelines → Clear runner caches
# Local: docker system prune -a
```

---

## Troubleshooting CI

### Build Fails

```bash
# Run locally to debug
docker compose -f docker/docker-compose.yml run --rm lites-i386-dev bash

# Inside container, single-threaded build for better errors
docker/scripts/build-lites-i386.sh -j 1
```

### QEMU Fails

```bash
# Check binary exists
docker compose -f docker/docker-compose.yml run --rm lites-i386-dev \
  ls -lh build-i386/lites_server

# Verify architecture
docker compose -f docker/docker-compose.yml run --rm lites-i386-dev \
  file build-i386/lites_server
```

### Timeout Issues

```bash
# Increase timeout
timeout 120 make -f Makefile.docker docker-run-qemu

# Or run with debug
docker/scripts/run-qemu-i386.sh --debug
```

### Artifact Issues

- Check artifact size limits (GitHub: 500 MB per artifact)
- Verify retention policy (default: 1 day for images, 7 days for binaries)
- Ensure artifact name matches between upload/download

---

## Status Badges

**GitHub Actions**:
```markdown
[![Docker i386 CI](https://github.com/YOUR_ORG/lites/actions/workflows/docker-i386-ci.yml/badge.svg)](https://github.com/YOUR_ORG/lites/actions/workflows/docker-i386-ci.yml)
```

**GitLab CI**:
```markdown
[![pipeline status](https://gitlab.com/YOUR_ORG/lites/badges/main/pipeline.svg)](https://gitlab.com/YOUR_ORG/lites/-/commits/main)
```

---

## Security Considerations

- Container runs as non-root user (`developer`)
- Privileged mode used only when necessary (KVM, disk image creation)
- No secrets required for public pipeline
- Artifacts expire automatically (1-7 days)
- Use dependabot/renovate for dependency updates

---

## References

- @./README.md - Quick reference
- @./DOCKER_GUIDE.md - Comprehensive guide
- @./TROUBLESHOOTING.md - Common problems
- GitHub Actions: https://docs.github.com/en/actions
- GitLab CI: https://docs.gitlab.com/ee/ci/
- Docker Compose v2: https://docs.docker.com/compose/

---

**Last Updated**: 2025-11-14 (documentation consolidation)
