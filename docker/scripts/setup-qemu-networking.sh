#!/usr/bin/env bash
##
## @file setup-qemu-networking.sh
## @brief Configure QEMU networking with best practices
## @details Sets up various QEMU networking modes: user, tap, bridge
##
## Usage: setup-qemu-networking.sh [MODE]
##
## Modes:
##   user    - User-mode networking (default, no root required)
##   tap     - TAP device (requires root, full network access)
##   bridge  - Bridge mode (requires root, advanced)
##

set -euo pipefail

NETWORK_MODE="${1:-user}"

## Configure user-mode networking (NAT)
## No root required, good for development
setup_user_networking() {
    cat <<'EOF'
User-mode networking configuration:

QEMU arguments:
  -netdev user,id=net0,hostfwd=tcp::2222-:22,hostfwd=tcp::8080-:80
  -device e1000,netdev=net0

Port forwards:
  - SSH: localhost:2222 -> guest:22
  - HTTP: localhost:8080 -> guest:80

Inside guest, configure networking:
  ifconfig eth0 10.0.2.15 netmask 255.255.255.0
  route add default gw 10.0.2.2
  
DNS servers: 10.0.2.3 (QEMU provides DNS)
EOF
}

## Configure TAP networking
## Requires root, provides full network access
setup_tap_networking() {
    if [[ $EUID -ne 0 ]]; then
        echo "Error: TAP networking requires root privileges" >&2
        exit 1
    fi
    
    TAP_DEVICE="${TAP_DEVICE:-tap0}"
    TAP_IP="${TAP_IP:-192.168.100.1}"
    TAP_NETMASK="${TAP_NETMASK:-255.255.255.0}"
    
    echo "Setting up TAP device: $TAP_DEVICE"
    
    # Create TAP device
    ip tuntap add dev "$TAP_DEVICE" mode tap user "$SUDO_USER"
    ip addr add "$TAP_IP/24" dev "$TAP_DEVICE"
    ip link set "$TAP_DEVICE" up
    
    # Enable IP forwarding
    echo 1 > /proc/sys/net/ipv4/ip_forward
    
    # Set up NAT for guest access to internet
    iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE
    iptables -A FORWARD -i "$TAP_DEVICE" -j ACCEPT
    iptables -A FORWARD -o "$TAP_DEVICE" -j ACCEPT
    
    cat <<EOF
TAP networking configured:

QEMU arguments:
  -netdev tap,id=net0,ifname=$TAP_DEVICE,script=no,downscript=no
  -device e1000,netdev=net0

Inside guest:
  ifconfig eth0 192.168.100.2 netmask 255.255.255.0
  route add default gw 192.168.100.1

To remove TAP device:
  ip link set $TAP_DEVICE down
  ip link delete $TAP_DEVICE
EOF
}

## Configure bridge networking
setup_bridge_networking() {
    if [[ $EUID -ne 0 ]]; then
        echo "Error: Bridge networking requires root privileges" >&2
        exit 1
    fi
    
    BRIDGE_NAME="${BRIDGE_NAME:-br0}"
    
    echo "Setting up bridge: $BRIDGE_NAME"
    
    # Create bridge
    ip link add name "$BRIDGE_NAME" type bridge
    ip link set "$BRIDGE_NAME" up
    
    cat <<EOF
Bridge networking configured:

QEMU arguments:
  -netdev bridge,id=net0,br=$BRIDGE_NAME
  -device e1000,netdev=net0

Note: Guest will get IP from your network DHCP server
EOF
}

## Main function
main() {
    case "$NETWORK_MODE" in
        user)
            setup_user_networking
            ;;
        tap)
            setup_tap_networking
            ;;
        bridge)
            setup_bridge_networking
            ;;
        *)
            echo "Error: Unknown network mode: $NETWORK_MODE" >&2
            echo "Use: user, tap, or bridge" >&2
            exit 1
            ;;
    esac
}

main
