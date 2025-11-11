# Placeholder for QEMU Images

Place your Lites i386 QEMU image here:

```
images/lites.qcow2
```

## Creating an Image

### Option 1: Build from Docker Environment (Recommended)

```bash
# Use the Docker i386 development environment
cd ../docker
make -f ../Makefile.docker docker-build-lites

# Build output will be in build-i386/
# See docker/README.md for creating bootable images
```

### Option 2: Use Existing Mach/Lites

```bash
# If you have an existing Mach/Lites installation

# Convert from raw format
qemu-img convert -f raw -O qcow2 input.img lites.qcow2

# Convert from VDI format
qemu-img convert -f vdi -O qcow2 input.vdi lites.qcow2
```

### Option 3: Create Minimal Image

```bash
# Create blank image
qemu-img create -f qcow2 lites.qcow2 2G

# Mount and configure with:
# - Mach microkernel
# - Lites server binary
# - Minimal filesystem
```

## Testing the Image

```bash
# Test boot
qemu-system-i386 \
  -m 4096 \
  -hda lites.qcow2 \
  -nographic

# Test with WebSocket serial
qemu-system-i386 \
  -m 4096 \
  -hda lites.qcow2 \
  -chardev socket,id=ws0,host=0.0.0.0,port=7681,server=on,websocket=on \
  -serial chardev:ws0 \
  -nographic
```

## Image Optimization

```bash
# Compress image
qemu-img convert -O qcow2 -c lites.qcow2 lites-compressed.qcow2

# Resize image
qemu-img resize lites.qcow2 +10G
```

## .gitignore

This directory is excluded from git by default (see `.gitignore`).
Images can be large (several GB) and should not be committed to the repository.
