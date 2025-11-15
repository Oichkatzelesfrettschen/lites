# QEMU Disk Images and Networking Guide

Complete guide for creating, managing, and using QEMU disk images with Lites i386 development.

## Table of Contents

1. [Disk Image Creation](#disk-image-creation)
2. [QEMU Best Practices](#qemu-best-practices)
3. [Network Configuration](#network-configuration)
4. [Docker Integration](#docker-integration)
5. [Troubleshooting](#troubleshooting)

## Disk Image Creation

### Quick Start

Create a basic QCOW2 disk image:

```bash
# Using the automated script
./docker/scripts/create-lites-disk.sh --size 2G --output lites.qcow2

# Manual creation with qemu-img
qemu-img create -f qcow2 -o lazy_refcounts=on,cluster_size=65536 lites.qcow2 2G
```

### Disk Formats Comparison

| Format | Pros | Cons | Use Case |
|--------|------|------|----------|
| **QCOW2** | Compression, snapshots, thin provisioning | Slower than raw | Development, testing |
| **RAW** | Best performance, simple | No compression, takes full space | Production, benchmarking |
| **VDI** | VirtualBox compatible | Limited features | VirtualBox users |
| **VMDK** | VMware compatible | Complex format | VMware users |

### QCOW2 Best Practices

**Recommended options:**
```bash
qemu-img create -f qcow2 \
    -o lazy_refcounts=on \      # Better performance
    -o cluster_size=65536 \      # 64KB clusters (optimal)
    -o compat=1.1 \              # Modern QEMU features
    -o compression_type=zlib \   # Optional compression
    lites.qcow2 2G
```

**Performance tuning:**
- `lazy_refcounts=on` - Reduces sync overhead (5-10% faster writes)
- `cluster_size=65536` - 64KB clusters optimal for most workloads
- `preallocation=metadata` - Faster initial writes
- `nocow=on` - Better performance on COW filesystems (btrfs)

### Disk Image Management

**Check disk image info:**
```bash
qemu-img info lites.qcow2
```

**Convert between formats:**
```bash
# QCOW2 to RAW
qemu-img convert -f qcow2 -O raw lites.qcow2 lites.raw

# RAW to QCOW2 with compression
qemu-img convert -f raw -O qcow2 -c lites.raw lites.qcow2
```

**Resize disk image:**
```bash
# Increase size
qemu-img resize lites.qcow2 +1G

# Check new size
qemu-img info lites.qcow2
```

**Create snapshots:**
```bash
# Create snapshot
qemu-img snapshot -c snapshot1 lites.qcow2

# List snapshots
qemu-img snapshot -l lites.qcow2

# Apply snapshot
qemu-img snapshot -a snapshot1 lites.qcow2
```

## QEMU Best Practices

### Basic QEMU Command

```bash
qemu-system-i386 \
    -m 256M \                                    # Memory
    -drive file=lites.qcow2,format=qcow2,if=ide \ # Disk
    -netdev user,id=net0 \                       # Network backend
    -device e1000,netdev=net0 \                  # Network device
    -serial stdio \                               # Serial console
    -nographic                                    # No GUI
```

### KVM Acceleration

**Check KVM availability:**
```bash
# Check if KVM is available
ls -l /dev/kvm

# Check if KVM module is loaded
lsmod | grep kvm
```

**Enable KVM:**
```bash
qemu-system-i386 \
    -enable-kvm \           # Enable KVM acceleration
    -cpu host \             # Pass through host CPU features
    -m 256M \
    -drive file=lites.qcow2,format=qcow2
```

**Performance improvement:** KVM provides 10-50x speedup over emulation.

### IDE vs VIRTIO Disk

**IDE (compatible):**
```bash
-drive file=lites.qcow2,format=qcow2,if=ide
```

**VIRTIO (faster, requires driver):**
```bash
-drive file=lites.qcow2,format=qcow2,if=virtio
```

### Memory Configuration

```bash
# Basic memory
-m 256M

# Memory with hugepages (requires host setup)
-m 256M -mem-path /dev/hugepages -mem-prealloc

# NUMA configuration
-numa node,memdev=mem0,cpus=0 -object memory-backend-ram,size=256M,id=mem0
```

### Serial Console Best Practices

**Serial to stdio (development):**
```bash
-serial stdio
```

**Serial to file:**
```bash
-serial file:serial.log
```

**Serial over Unix socket:**
```bash
-serial unix:/tmp/lites-serial.sock,server,nowait
```

**Serial over TCP:**
```bash
-serial tcp::4444,server,nowait
```

**WebSocket serial (for web terminal):**
```bash
-chardev socket,id=ws0,host=0.0.0.0,port=7681,server=on,websocket=on \
-serial chardev:ws0
```

## Network Configuration

### User-Mode Networking (NAT)

**Advantages:**
- No root/admin required
- Works out of the box
- Port forwarding available

**Disadvantages:**
- NAT only (no direct inbound connections)
- Slightly slower than TAP

**Configuration:**
```bash
qemu-system-i386 \
    -netdev user,id=net0,hostfwd=tcp::2222-:22,hostfwd=tcp::8080-:80 \
    -device e1000,netdev=net0
```

**Port forwards:**
- SSH: `localhost:2222` → `guest:22`
- HTTP: `localhost:8080` → `guest:80`

**Guest network config:**
```bash
# Inside guest OS
ifconfig eth0 10.0.2.15 netmask 255.255.255.0
route add default gw 10.0.2.2
# DNS: 10.0.2.3 (provided by QEMU)
```

### TAP Networking (Full Access)

**Advantages:**
- Full network access
- Better performance
- Guest appears on network

**Disadvantages:**
- Requires root privileges
- More complex setup

**Setup script:**
```bash
# Run setup script (requires root)
sudo ./docker/scripts/setup-qemu-networking.sh tap

# Or manual setup
sudo ip tuntap add dev tap0 mode tap user $USER
sudo ip addr add 192.168.100.1/24 dev tap0
sudo ip link set tap0 up
```

**QEMU command:**
```bash
qemu-system-i386 \
    -netdev tap,id=net0,ifname=tap0,script=no,downscript=no \
    -device e1000,netdev=net0
```

**Guest config:**
```bash
ifconfig eth0 192.168.100.2 netmask 255.255.255.0
route add default gw 192.168.100.1
```

### Network Device Choices

| Device | Speed | Compatibility | Use Case |
|--------|-------|---------------|----------|
| `e1000` | Good | Excellent | Default choice |
| `rtl8139` | Medium | Good | Old systems |
| `virtio-net` | Excellent | Requires driver | Performance |
| `ne2k_pci` | Low | Universal | Legacy |

## Docker Integration

### Complete Workflow

```bash
# Run complete workflow
./docker/scripts/docker-build-and-run.sh all

# Individual steps
./docker/scripts/docker-build-and-run.sh build    # Build Docker image
./docker/scripts/docker-build-and-run.sh compile  # Build Lites
./docker/scripts/docker-build-and-run.sh disk     # Create disk image
./docker/scripts/docker-build-and-run.sh run      # Run in QEMU
```

### Docker with KVM

**Enable KVM in Docker:**
```yaml
# docker-compose.yml
services:
  qemu-runner:
    image: lites-i386-dev
    devices:
      - /dev/kvm:/dev/kvm
    privileged: false  # Not needed with device mapping
```

**Docker command:**
```bash
docker run --device=/dev/kvm:/dev/kvm lites-i386-dev qemu-system-i386 -enable-kvm ...
```

### Volume Mounting

```yaml
services:
  dev:
    volumes:
      - .:/workspace            # Source code
      - build-cache:/workspace/.ccache  # Build cache
      - qemu-images:/workspace/images   # Disk images

volumes:
  build-cache:
  qemu-images:
```

## Troubleshooting

### KVM Not Available in Docker

**Problem:** `/dev/kvm: Permission denied`

**Solutions:**
```bash
# 1. Add device mapping
docker run --device=/dev/kvm:/dev/kvm ...

# 2. Check host KVM
ls -l /dev/kvm
# Should show: crw-rw-rw- ... kvm

# 3. Add user to kvm group (host)
sudo usermod -a -G kvm $USER
```

### Disk Image Corruption

**Problem:** QCOW2 image corrupted after crash

**Solution:**
```bash
# Check and repair
qemu-img check -r all lites.qcow2

# If unrepairable, restore from backup
cp lites.qcow2.backup lites.qcow2
```

### Network Not Working in Guest

**Checklist:**
1. Check QEMU network device: `-device e1000,netdev=net0`
2. Check network backend: `-netdev user,id=net0`
3. Inside guest: `ifconfig -a` (is eth0 present?)
4. Check IP config: `ifconfig eth0 10.0.2.15 netmask 255.255.255.0`
5. Check gateway: `route add default gw 10.0.2.2`
6. Test: `ping 10.0.2.2` (QEMU host gateway)

### Slow Performance

**Checklist:**
1. Enable KVM: `-enable-kvm -cpu host`
2. Use VIRTIO devices where possible
3. Increase memory: `-m 512M` or more
4. Use RAW format for disk (faster than QCOW2)
5. Check host system load
6. Use SSD for disk images
7. Enable `cache=writeback`: `-drive file=...,cache=writeback`

### Serial Console Not Working

**Solutions:**
```bash
# 1. Check QEMU args
-serial stdio

# 2. Guest kernel boot args (add to grub/lilo)
console=ttyS0,115200

# 3. Guest /etc/inittab
T0:23:respawn:/sbin/getty -L ttyS0 115200 vt100

# 4. Test with telnet
-serial tcp::4444,server,nowait
telnet localhost 4444
```

## Advanced Topics

### Multi-disk Setup

```bash
qemu-system-i386 \
    -drive file=lites-root.qcow2,format=qcow2,if=ide,index=0 \
    -drive file=lites-data.qcow2,format=qcow2,if=ide,index=1
```

### Backing Files (Copy-on-Write)

```bash
# Create base image
qemu-img create -f qcow2 base.qcow2 2G

# Create instance with base as backing file
qemu-img create -f qcow2 -b base.qcow2 -F qcow2 instance1.qcow2

# Instance only stores differences from base
```

### QEMU Monitor

```bash
# Access QEMU monitor
-monitor telnet:127.0.0.1:5555,server,nowait

# In another terminal
telnet localhost 5555

# Monitor commands
info block          # Show disks
info network        # Show network
savevm snapshot1    # Create snapshot
loadvm snapshot1    # Load snapshot
quit                # Exit QEMU
```

## References

- [QEMU Documentation](https://www.qemu.org/documentation/)
- [QEMU Disk Images Guide](https://qemu.readthedocs.io/en/latest/system/images.html)
- [KVM Documentation](https://www.linux-kvm.org/page/Documents)
- [QCOW2 Format Specification](https://github.com/qemu/qemu/blob/master/docs/interop/qcow2.txt)
