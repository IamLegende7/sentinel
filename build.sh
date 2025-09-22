#!/bin/bash
set -e

# Detect distro
if [ -f /etc/debian_version ]; then
    DISTRO="debian"
elif [ -f /etc/arch-release ]; then
    DISTRO="arch"
else
    echo "Unsupported Linux distribution. Please install dependencies manually."
    exit 1
fi

echo "Detected distro: $DISTRO"

# Install dependencies
if [ "$DISTRO" = "debian" ]; then
    sudo apt update
    sudo apt install -y build-essential make g++ libsdl3-dev libsdl3-image-dev
elif [ "$DISTRO" = "arch" ]; then
    sudo pacman -Sy --noconfirm base-devel gcc make
fi

# Build
make clean all