# Mach Headers Setup Guide

## Overview

Building Lites requires Mach kernel headers. This guide explains how the Mach headers are integrated into the Docker development environment and how to set them up manually if needed.

## Docker Image (Recommended)

The `lites-i386-dev` Docker image automatically includes Mach headers from the [osfmk6.1](https://github.com/nmartin0/osfmk6.1) repository.

**Location in container:** `/opt/mach/osfmk/`

**Environment variables set:**
- `LITES_MACH_DIR=/opt/mach/osfmk/kernel/src`
- `MACH_INCLUDE_DIR=/opt/mach/osfmk/kernel/src`

### Verify Headers

```bash
# Check headers are present
docker compose -f docker/docker-compose.yml run --rm lites-i386-dev \
  bash -c "ls -la /opt/mach/osfmk/kernel/src | head -20"

# Count header files
docker compose -f docker/docker-compose.yml run --rm lites-i386-dev \
  bash -c "find /opt/mach/osfmk -name '*.h' | wc -l"
```

## Manual Setup Options

If you're not using Docker or need to set up headers manually:

### Option 1: Use Docker Image Headers

If you're inside the Docker container:

```bash
# Automatic setup from Docker image
docker/scripts/setup-mach-headers.sh --docker
```

This copies headers from `/opt/mach/osfmk` to `localmach/include`.

### Option 2: Clone from GitHub

```bash
# Clone fresh headers from nmartin0's repository
docker/scripts/setup-mach-headers.sh --clone
```

This clones [osfmk6.1](https://github.com/nmartin0/osfmk6.1) and extracts headers to `localmach/include`.

### Option 3: Use Local Installation

If you have a local Mach installation:

```bash
# Use headers from local path
docker/scripts/setup-mach-headers.sh --local /path/to/osfmk
```

### Option 4: Manual Git Clone

```bash
# Clone manually
git clone --depth 1 https://github.com/nmartin0/osfmk6.1.git openmach

# Set environment variable
export LITES_MACH_DIR="$(pwd)/openmach/kernel/src"

# Build
cmake -B build -DLITES_MACH_DIR="${LITES_MACH_DIR}"
cmake --build build
```

## Build Integration

The build system automatically detects Mach headers in the following order:

1. `$LITES_MACH_DIR` environment variable
2. `/opt/mach/osfmk/kernel/src` (Docker image)
3. `localmach/include` (from setup script)
4. `openmach/include` (git submodule)

The `docker/scripts/build-lites-i386.sh` script automatically checks for headers and provides helpful error messages if they're missing.

## Header Sources

### Primary Source: osfmk6.1

- **Repository:** https://github.com/nmartin0/osfmk6.1
- **Description:** OSF Mach 6.1 - Open Software Foundation's Mach 3.0 microkernel
- **License:** BSD-style (unencumbered)

### Alternative Sources

Other Mach repositories from nmartin0 that contain headers:

- **osfmk7.3:** https://github.com/nmartin0/osfmk7.3
- **mach-mk74:** https://github.com/nmartin0/mach-mk74
- **mach4ALPHA:** https://github.com/nmartin0/mach4ALPHA

## Troubleshooting

### Headers Not Found

If CMake reports missing Mach headers:

```
CMake Error: Mach headers not found. Set LITES_MACH_DIR or add the openmach submodule
```

**Solution:**
```bash
# Inside Docker container
docker/scripts/setup-mach-headers.sh --docker

# Outside Docker
docker/scripts/setup-mach-headers.sh --clone

# Or set manually
export LITES_MACH_DIR=/path/to/mach/headers
```

### Docker Image Missing Headers

If `/opt/mach/osfmk` doesn't exist in the Docker image:

**Solution:** Rebuild the Docker image
```bash
docker compose -f docker/docker-compose.yml build --no-cache lites-i386-dev
```

### Build Fails with Header Errors

If the build fails with missing Mach header errors:

1. Verify headers are present:
   ```bash
   ls -la ${LITES_MACH_DIR}/
   find ${LITES_MACH_DIR} -name "*.h" | head -10
   ```

2. Check environment variables:
   ```bash
   echo $LITES_MACH_DIR
   echo $MACH_INCLUDE_DIR
   ```

3. Ensure CMake uses the correct path:
   ```bash
   cmake -B build -DLITES_MACH_DIR="${LITES_MACH_DIR}"
   ```

## CI/CD Integration

The GitHub Actions workflow automatically:

1. Builds Docker image with Mach headers pre-installed
2. Verifies headers are present in the image
3. Attempts to configure CMake with the headers
4. Reports header availability and count

See `.github/workflows/docker-i386-ci.yml` for implementation details.

## More Information

- **Lites Documentation:** [docs/](../docs/)
- **Docker Overview:** [docker/OVERVIEW.md](OVERVIEW.md)
- **Getting Started:** [docker/GETTING_STARTED.md](GETTING_STARTED.md)
- **Build System:** [CMakeLists.txt](../CMakeLists.txt) (lines 36-56)
