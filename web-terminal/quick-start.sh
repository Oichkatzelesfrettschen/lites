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
if [ ! -f "images/lites.qcow2" ]; then
    echo "⚠ No QEMU image found at images/lites.qcow2"
    echo ""
    echo "You need a Lites i386 QEMU image to continue."
    echo ""
    echo "Options:"
    echo "  1. Build using the docker/ i386 environment"
    echo "  2. Convert an existing Mach/Lites image"
    echo "  3. Create a custom bootable image"
    echo ""
    echo "See images/README.md for detailed instructions"
    echo ""
    read -p "Do you want to continue without an image? (y/N) " -n 1 -r
    echo ""
    
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "Please place your QEMU image at images/lites.qcow2"
        exit 1
    fi
    
    echo "⚠ Warning: Continuing without image - backend will fail to start"
    echo ""
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
echo "  4. Login to Lites i386"
echo ""
echo "Commands:"
echo "  View logs:  docker compose logs -f"
echo "  Stop:       docker compose down"
echo "  Restart:    docker compose restart"
echo ""
echo "SSH access: ssh -p 2222 root@localhost"
echo ""
