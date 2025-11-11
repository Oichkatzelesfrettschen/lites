#!/usr/bin/env bash
set -euo pipefail

##
# @file quick-start.sh
# @brief Quick start script for Lites Web Terminal
#
# This script helps you get the web terminal up and running quickly
##

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "=== Lites Web Terminal Quick Start ==="
echo ""

# Check Docker
if ! command -v docker >/dev/null 2>&1; then
    echo "Error: Docker is not installed"
    echo "Please install Docker: https://docs.docker.com/get-docker/"
    exit 1
fi

# Check Docker Compose
if ! docker compose version >/dev/null 2>&1; then
    echo "Error: Docker Compose v2 is not available"
    echo "Please install Docker Compose v2"
    exit 1
fi

echo "✓ Docker is installed"
echo "✓ Docker Compose v2 is available"
echo ""

# Check for QEMU image
if [ ! -f "images/hurd.qcow2" ]; then
    echo "⚠ No QEMU image found at images/hurd.qcow2"
    echo ""
    echo "You need a Debian GNU/Hurd QEMU image to continue."
    echo ""
    echo "Options:"
    echo "  1. Download a pre-built image"
    echo "  2. Build one from ISO"
    echo "  3. Convert an existing image"
    echo ""
    echo "See images/README.md for detailed instructions"
    echo ""
    read -p "Do you want to download a pre-built image? (y/N) " -n 1 -r
    echo ""
    
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo "Downloading Debian GNU/Hurd image..."
        echo "(This may take several minutes depending on your connection)"
        echo ""
        
        mkdir -p images
        cd images
        
        # Try to download
        if wget -c https://cdimage.debian.org/cdimage/ports/latest/hurd-i386/debian-hurd.img.tar.gz; then
            echo "Extracting..."
            tar xzf debian-hurd.img.tar.gz
            mv debian-hurd.img hurd.qcow2
            rm debian-hurd.img.tar.gz
            echo "✓ Image ready"
        else
            echo "Error: Download failed"
            echo "Please manually place a QEMU image at images/hurd.qcow2"
            exit 1
        fi
        
        cd "$SCRIPT_DIR"
    else
        echo "Please place your QEMU image at images/hurd.qcow2"
        exit 1
    fi
fi

echo "✓ QEMU image found"
echo ""

# Build Docker image
echo "Building Docker image..."
docker compose build

echo "✓ Docker image built"
echo ""

# Start services
echo "Starting QEMU backend..."
docker compose up -d

echo ""
echo "✓ Backend is starting..."
echo ""

# Wait for service to be ready
echo "Waiting for QEMU to start (this may take 30-60 seconds)..."
sleep 10

# Show status
docker compose ps

echo ""
echo "=== Quick Start Complete ==="
echo ""
echo "Backend is running!"
echo ""
echo "Next steps:"
echo "  1. Open index.html in your browser (or deploy to GitHub Pages)"
echo "  2. Enter WebSocket URL: ws://localhost:7681"
echo "  3. Click 'Connect'"
echo "  4. Login to Debian GNU/Hurd"
echo ""
echo "Commands:"
echo "  View logs:  docker compose logs -f"
echo "  Stop:       docker compose down"
echo "  Restart:    docker compose restart"
echo ""
echo "SSH access: ssh -p 2222 root@localhost"
echo ""
