#!/usr/bin/env bash
set -euo pipefail

##
# @file entrypoint.sh
# @brief QEMU entrypoint for Lites i386 WebSocket serial console
#
# This script launches QEMU with a WebSocket-enabled serial console
# that can be accessed from a web browser using xterm.js
##

# Configuration from environment variables
IMG="${LITES_IMG:-/lites/lites.qcow2}"
RAM="${LITES_RAM:-4096}"
CPUS="${LITES_CPUS:-1}"
WS_PORT="${WS_PORT:-7681}"
SSH_FWD_HOSTPORT="${SSH_FWD_HOSTPORT:-2222}"
VNC_PORT="${VNC_PORT:-5900}"

echo "=== Lites i386 QEMU Backend ==="
echo "Image: $IMG"
echo "RAM: ${RAM}M"
echo "CPUs: $CPUS"
echo "WebSocket Serial Port: $WS_PORT"
echo "SSH Forward Port: $SSH_FWD_HOSTPORT"
echo "VNC Port: $VNC_PORT"
echo ""

# Check if image exists
if [ ! -f "$IMG" ]; then
    echo "Error: QEMU image not found: $IMG"
    echo ""
    echo "Please either:"
    echo "  1. Mount a volume with your image: -v /path/to/image.qcow2:/lites/lites.qcow2"
    echo "  2. Build a custom image with COPY in Dockerfile"
    echo "  3. Download an image at startup (see README)"
    echo ""
    exit 1
fi

# Detect KVM availability
KVM_OPTS=""
if [ -e /dev/kvm ] && [ -w /dev/kvm ]; then
    echo "✓ KVM acceleration enabled"
    KVM_OPTS="-accel kvm -cpu host"
else
    echo "⚠ KVM not available, using TCG (slower)"
    KVM_OPTS="-cpu qemu32"
fi

echo ""
echo "Starting QEMU..."
echo "Connect via web terminal at: ws://localhost:$WS_PORT"
echo "SSH available at: localhost:$SSH_FWD_HOSTPORT"
echo ""

# Launch QEMU with WebSocket serial console
exec qemu-system-i386 \
    $KVM_OPTS \
    -machine pc \
    -m "${RAM}" \
    -smp "${CPUS}" \
    -device ich9-ahci,id=ach0 \
    -drive if=none,file="${IMG}",format=qcow2,id=d0 \
    -device ide-hd,drive=d0,bus=ach0.0 \
    -device e1000,netdev=net0 \
    -netdev user,id=net0,hostfwd=tcp::${SSH_FWD_HOSTPORT}-:22 \
    -chardev socket,id=ws0,host=0.0.0.0,port="${WS_PORT}",server=on,websocket=on \
    -serial chardev:ws0 \
    -nographic
