# GitHub Actions CI/CD for Docker i386 Development

This document describes the GitHub Actions CI/CD pipeline for the Lites i386 Docker development environment.

## Overview

The CI/CD pipeline is defined in `.github/workflows/docker-i386-ci.yml` and provides comprehensive testing of the Docker-based development environment using **Docker Compose v2** (not legacy v1).

## Workflow Structure

The pipeline consists of 6 main jobs that run in sequence:

```
┌─────────────────┐
│  docker-lint    │ ← Validate scripts, YAML, Makefile
└────────┬────────┘
         │
         ↓
┌─────────────────┐
│  docker-build   │ ← Build Docker image, verify tools
└────────┬────────┘
         │
    ┌────┴────┬──────────────────────┐
    ↓         ↓                      ↓
┌────────┐ ┌──────────┐  ┌──────────────────┐
│ build  │ │  test    │  │ makefile-targets │
│ lites  │ │  docker  │  │                  │
└────┬───┘ └────┬─────┘  └────┬─────────────┘
     │          │              │
     └──────────┴──────────────┘
                │
                ↓
    ┌───────────────────┐
    │  integration      │ ← Full workflow test
    └─────────┬─────────┘
              │
              ↓
    ┌───────────────────┐
    │  summary          │ ← Report results
    └───────────────────┘
```

## Jobs Description

### 1. docker-lint
**Purpose:** Validate all scripts and configuration files

**Steps:**
- Install shellcheck
- Lint bash scripts with shellcheck
- Validate bash syntax with `bash -n`
- Validate YAML files with Python
- Verify Makefile syntax

**Exit Criteria:** All linting passes with no errors

### 2. docker-build
**Purpose:** Build the Docker image and verify tools

**Steps:**
- Set up Docker Buildx
- Display Docker Compose version (confirms v2)
- Build Docker image using `docker compose` (v2 syntax)
- Verify image exists
- Test container starts
- Verify development tools (GCC, MIG, QEMU, CMake, Ninja)
- Export image as artifact for downstream jobs

**Exit Criteria:** Image builds successfully and contains all required tools

### 3. docker-build-lites
**Purpose:** Test the Lites build process

**Steps:**
- Download Docker image artifact
- Load image
- Verify Docker Compose v2 syntax
- Run build script (smoke test)
- Verify build artifacts structure

**Exit Criteria:** Build process can be invoked successfully

### 4. docker-test
**Purpose:** Test the Docker environment functionality

**Steps:**
- Run environment tests
  - Test GCC multilib compilation
  - Test MIG availability
  - Test QEMU functionality
- Test build scripts syntax

**Exit Criteria:** All environment tests pass

### 5. docker-integration
**Purpose:** End-to-end integration testing

**Steps:**
- Full workflow simulation
- Test all helper scripts
- Verify Docker Compose v2 commands
- Validate compose file syntax

**Exit Criteria:** Complete workflow executes successfully

### 6. docker-makefile-targets
**Purpose:** Test Makefile targets

**Steps:**
- Test `make help`
- Test `make docker-build`
- Test container shell access

**Exit Criteria:** All Makefile targets work correctly

### 7. summary
**Purpose:** Report overall pipeline status

**Steps:**
- Check all job results
- Display summary
- Exit with error if any job failed

**Exit Criteria:** All jobs passed

## Docker Compose v2 Usage

The pipeline explicitly uses Docker Compose v2 syntax throughout:

```bash
# v2 syntax (correct)
docker compose version
docker compose -f docker/docker-compose.yml build
docker compose -f docker/docker-compose.yml run --rm service command

# v1 syntax (legacy, NOT used)
docker-compose version
docker-compose -f docker/docker-compose.yml build
```

### Why Docker Compose v2?

1. **Modern Standard:** v2 is the current Docker standard
2. **Built-in:** Included with Docker Engine 20.10+
3. **Better Performance:** Improved build caching and parallelization
4. **Active Development:** v1 is deprecated

## Triggering the Pipeline

The workflow runs on:

- **Push** to `main`, `develop`, or any `copilot/**` branch
- **Pull Request** to `main` or `develop`
- **Manual trigger** via workflow_dispatch

## Environment Variables

```yaml
DOCKER_BUILDKIT: 1              # Enable BuildKit
COMPOSE_DOCKER_CLI_BUILD: 1     # Use BuildKit for Compose
```

## Artifacts

The pipeline produces:

- **lites-i386-dev-image** - Docker image (1-day retention)
  - Used by downstream jobs
  - Exported as `lites-i386-dev.tar.gz`

## Local Testing

You can test the pipeline locally:

```bash
# Install act (GitHub Actions local runner)
# https://github.com/nektos/act

# Run the full workflow
act -W .github/workflows/docker-i386-ci.yml

# Run specific job
act -W .github/workflows/docker-i386-ci.yml -j docker-build

# Run with secrets
act -W .github/workflows/docker-i386-ci.yml -s GITHUB_TOKEN=...
```

## Troubleshooting

### Pipeline Fails at docker-build

**Symptom:** Image build fails

**Solution:**
1. Check Dockerfile syntax: `docker build -f docker/Dockerfile.i386-dev .`
2. Verify base image is accessible: `docker pull debian:bookworm-slim`
3. Check disk space: `df -h`

### Pipeline Fails at docker-test

**Symptom:** Environment tests fail

**Solution:**
1. Run tests locally: `docker compose -f docker/docker-compose.yml run --rm lites-i386-dev bash`
2. Verify tools are installed in Dockerfile
3. Check for missing packages

### Docker Compose v2 Not Found

**Symptom:** `docker compose: command not found`

**Solution:**
1. Update Docker Engine to 20.10+
2. Install Docker Compose v2: https://docs.docker.com/compose/install/
3. Verify: `docker compose version`

### Artifact Upload/Download Fails

**Symptom:** Artifact operations fail

**Solution:**
1. Check GitHub Actions artifact limits (quota)
2. Verify artifact name matches between upload/download
3. Check retention policy (default: 1 day for this pipeline)

## Performance

Typical execution times:

| Job | Time | Notes |
|-----|------|-------|
| docker-lint | 30s | Fast, no Docker operations |
| docker-build | 5-10min | First build, then cached |
| docker-build-lites | 1-2min | Uses cached image |
| docker-test | 1-2min | Uses cached image |
| docker-integration | 1-2min | Uses cached image |
| docker-makefile-targets | 1-2min | Uses cached image |
| summary | 10s | Just reporting |

**Total:** ~10-20 minutes for full pipeline (first run)
**Subsequent:** ~5-10 minutes (with caching)

## Caching

The pipeline uses Docker layer caching via GitHub Actions cache:

- Base images cached
- Package installations cached
- Build context cached

To clear cache:
1. Go to repository Settings → Actions → Caches
2. Delete relevant caches
3. Re-run workflow

## Status Badges

Add to your README:

```markdown
[![Docker i386 CI/CD](https://github.com/YOUR_ORG/lites/actions/workflows/docker-i386-ci.yml/badge.svg)](https://github.com/YOUR_ORG/lites/actions/workflows/docker-i386-ci.yml)
```

## Extending the Pipeline

To add new jobs:

1. Add job definition in `.github/workflows/docker-i386-ci.yml`
2. Set `needs:` to specify dependencies
3. Use Docker Compose v2 syntax
4. Add to summary job's needs list

Example:

```yaml
my-new-job:
  name: My Custom Test
  runs-on: ubuntu-latest
  needs: docker-build
  steps:
    - uses: actions/checkout@v4
    - uses: actions/download-artifact@v4
      with:
        name: lites-i386-dev-image
    - run: docker load < lites-i386-dev.tar.gz
    - run: docker compose -f docker/docker-compose.yml run --rm lites-i386-dev my-command
```

## Security Considerations

- Container runs as non-root user (`developer`)
- Privileged mode used only when necessary (KVM)
- No secrets required for public pipeline
- Artifacts expire after 1 day

## References

- [Docker Compose v2 Documentation](https://docs.docker.com/compose/)
- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [Docker Buildx](https://docs.docker.com/buildx/working-with-buildx/)
- [Main Docker README](../README.md)

## Support

For issues with the CI/CD pipeline:
1. Check the Actions tab for logs
2. Review this documentation
3. Open an issue with the `ci/cd` label
