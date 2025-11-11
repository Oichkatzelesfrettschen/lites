# Placeholder for QEMU Images

Place your Debian GNU/Hurd QEMU image here:

```
images/hurd.qcow2
```

## Creating an Image

### Option 1: Download Pre-built

```bash
# Download Debian GNU/Hurd (if available)
wget https://cdimage.debian.org/cdimage/ports/latest/hurd-i386/debian-hurd.img.tar.gz
tar xzf debian-hurd.img.tar.gz
mv debian-hurd.img hurd.qcow2
```

### Option 2: Build from Installer

```bash
# Create blank image
qemu-img create -f qcow2 hurd.qcow2 20G

# Download installer ISO
wget https://cdimage.debian.org/cdimage/ports/latest/hurd-i386/debian-testing-hurd-i386-NETINST-1.iso

# Install
qemu-system-i386 \
  -m 4096 \
  -cdrom debian-testing-hurd-i386-NETINST-1.iso \
  -hda hurd.qcow2 \
  -boot d \
  -nographic
```

### Option 3: Convert Existing Image

```bash
# If you have a raw image
qemu-img convert -f raw -O qcow2 input.img hurd.qcow2

# If you have a VDI image
qemu-img convert -f vdi -O qcow2 input.vdi hurd.qcow2
```

## Testing the Image

```bash
# Test boot
qemu-system-i386 \
  -m 4096 \
  -hda hurd.qcow2 \
  -nographic

# Test with WebSocket serial
qemu-system-i386 \
  -m 4096 \
  -hda hurd.qcow2 \
  -chardev socket,id=ws0,host=0.0.0.0,port=7681,server=on,websocket=on \
  -serial chardev:ws0 \
  -nographic
```

## Image Optimization

```bash
# Compress image
qemu-img convert -O qcow2 -c hurd.qcow2 hurd-compressed.qcow2

# Resize image
qemu-img resize hurd.qcow2 +10G
```

## .gitignore

This directory is excluded from git by default (see `.gitignore`).
Images can be large (several GB) and should not be committed to the repository.
