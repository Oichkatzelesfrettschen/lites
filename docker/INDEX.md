# Docker i386 Development Environment - Index

This is the main index for the Lites Docker-based i386 development environment.

## What is This?

A complete, containerized development environment for building and running Lites on i386 architecture inside QEMU, all within Docker. This provides:

- **Zero-setup development**: All tools pre-installed and configured
- **Reproducible builds**: Same environment on any platform
- **Nested virtualization**: QEMU i386 inside Docker
- **Full documentation**: Step-by-step guides and references

## Quick Navigation

### For New Users

Start here if this is your first time:

1. **[GETTING_STARTED.md](GETTING_STARTED.md)** - Complete beginner's guide
   - Prerequisites check
   - Step-by-step setup
   - First build and run
   - Common workflows
   - Troubleshooting

### For Understanding the System

Learn about the architecture:

2. **[OVERVIEW.md](OVERVIEW.md)** - High-level architecture overview
   - System architecture diagrams
   - Component descriptions
   - Comparison with alternatives
   - Use case guidance

3. **[WORKFLOW.md](WORKFLOW.md)** - Visual workflow documentation
   - ASCII architecture diagrams
   - Development workflows
   - Tool interactions
   - Performance characteristics

### For Detailed Reference

Technical details and options:

4. **[README.md](README.md)** - Complete technical reference
   - All features and options
   - Script documentation
   - Advanced usage
   - Troubleshooting details

5. **[CI_CD_WORKFLOW.md](CI_CD_WORKFLOW.md)** - CI/CD integration guide
   - Complete workflow documentation
   - Build process details
   - QEMU execution flow
   - Environment variables

6. **[GITHUB_ACTIONS.md](GITHUB_ACTIONS.md)** - GitHub Actions CI/CD
   - Automated testing pipeline
   - Docker Compose v2 usage
   - Job descriptions
   - Troubleshooting CI issues

## File Structure

```
docker/
├── INDEX.md                      ← You are here
├── GETTING_STARTED.md           ← Start here for tutorials
├── OVERVIEW.md                  ← Architecture overview
├── WORKFLOW.md                  ← Visual diagrams
├── README.md                    ← Technical reference
├── CI_CD_WORKFLOW.md            ← CI/CD integration
├── GITHUB_ACTIONS.md            ← GitHub Actions setup
│
├── Dockerfile.i386-dev          ← Docker image definition
├── docker-compose.yml           ← Container orchestration (v2)
├── docker-compose.yml           ← Container orchestration
│
└── scripts/
    ├── build-lites-i386.sh      ← Build automation
    ├── run-qemu-i386.sh         ← QEMU launcher
    ├── docker-shell.sh          ← Container entry helper
    ├── test-docker-build.sh     ← Environment validation
    └── setup-hurd-dev.sh        ← GNU/Hurd setup (optional)
```

## Quick Start Commands

```bash
# Complete workflow (first time)
make -f Makefile.docker docker-all

# Enter development container
make -f Makefile.docker docker-shell

# Build Lites for i386
make -f Makefile.docker docker-build-lites

# Run in QEMU
make -f Makefile.docker docker-run-qemu

# Debug with GDB
make -f Makefile.docker docker-gdb
```

## Documentation by Task

### I want to...

- **Get started quickly** → [GETTING_STARTED.md](GETTING_STARTED.md)
- **Understand the architecture** → [OVERVIEW.md](OVERVIEW.md)
- **See visual diagrams** → [WORKFLOW.md](WORKFLOW.md)
- **Find all options** → [README.md](README.md)
- **Debug with GDB** → [README.md#debugging](README.md#debugging)
- **Use GNU/Hurd** → [scripts/setup-hurd-dev.sh](scripts/setup-hurd-dev.sh)
- **Validate my setup** → [scripts/test-docker-build.sh](scripts/test-docker-build.sh)
- **Customize the environment** → [Dockerfile.i386-dev](Dockerfile.i386-dev)

### I need help with...

- **Installation issues** → [GETTING_STARTED.md#prerequisites](GETTING_STARTED.md#prerequisites)
- **Build failures** → [GETTING_STARTED.md#troubleshooting](GETTING_STARTED.md#troubleshooting)
- **QEMU problems** → [README.md#qemu-troubleshooting](README.md#qemu-troubleshooting)
- **KVM acceleration** → [README.md#kvm-not-available](README.md#kvm-not-available)
- **Performance optimization** → [OVERVIEW.md#performance-tips](OVERVIEW.md#performance-tips)

## Common Workflows

### Daily Development

```bash
# 1. Edit code on host
vim core/myfile.c

# 2. Rebuild in container
make -f Makefile.docker docker-build-lites

# 3. Test in QEMU
make -f Makefile.docker docker-run-qemu
```

### Debugging

```bash
# Terminal 1: Start QEMU with GDB
make -f Makefile.docker docker-gdb

# Terminal 2: Connect GDB
docker/scripts/docker-shell.sh
gdb build-i386/lites_server
(gdb) target remote localhost:1234
```

### Testing

```bash
# Run full test suite
make -f Makefile.docker docker-test

# Validate environment
docker/scripts/test-docker-build.sh
```

## Architecture at a Glance

```
Host System
  └─ Docker Container (Debian + Tools)
      └─ QEMU i386
          └─ Lites Kernel (4.4BSD on Mach)
```

## Key Features

- ✅ **Pre-configured i386 toolchain** - GCC, MIG, QEMU
- ✅ **Automated build scripts** - One command to build
- ✅ **QEMU integration** - Test immediately after build
- ✅ **KVM acceleration** - 10-50x faster emulation
- ✅ **GDB debugging** - Remote kernel debugging
- ✅ **GNU/Hurd option** - Native Mach/Hurd development
- ✅ **Cross-platform** - Works on Linux, macOS, Windows
- ✅ **Persistent caching** - Fast incremental builds

## System Requirements

| Component | Minimum | Recommended |
|-----------|---------|-------------|
| Docker | 20.10+ | Latest |
| RAM | 4 GB | 8 GB+ |
| Disk | 5 GB | 10 GB+ |
| CPU | 2 cores | 4+ cores |

## Support and Resources

- **Main Repository**: [Lites on GitHub](https://github.com/Oichkatzelesfrettschen/lites)
- **Issue Tracker**: [GitHub Issues](https://github.com/Oichkatzelesfrettschen/lites/issues)
- **Discussions**: [GitHub Discussions](https://github.com/Oichkatzelesfrettschen/lites/discussions)

## Related Documentation

Outside the `docker/` directory:

- [Main README](../README.md) - Repository overview
- [Setup Guide](../docs/setup.md) - Native host setup
- [Architecture](../docs/ARCHITECTURE.md) - Lites architecture
- [Makefile.docker](../Makefile.docker) - Make targets reference

## Contributing

Improvements to the Docker environment are welcome! See the main repository for contribution guidelines.

## License

Same as the main Lites repository. See [LICENSE](../LICENSE).

---

**Need help?** Start with [GETTING_STARTED.md](GETTING_STARTED.md) or open an issue on GitHub.

**Ready to dive in?** Run `make -f Makefile.docker docker-all` and you'll be building Lites in minutes!
