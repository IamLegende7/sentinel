#!/bin/bash
set -e

echo "THIS SCRIPT HAS ONLY BEEN TESTED ON ARCH LINUX. PROCEED WITH CAUTION!"
read

cd "$(dirname "$0")"
cd ..

# Detect distro
if [[ "$OSTYPE" != "linux-gnu"* ]]; then
    echo "This bash script is only for Linux. Use the build script for your OS ($OSTYPE)"
    exit 1
fi

if [ -f /etc/debian_version ]; then
    DISTRO="debian"
elif [ -f /etc/arch-release ]; then
    DISTRO="arch"
elif [ -f /etc/fedora-release ]; then
    DISTRO="fedora"
elif [ -f /etc/redhat-release ]; then
    DISTRO="centos"
elif [ -f /etc/os-release ] && grep -qi "suse" /etc/os-release; then
    DISTRO="opensuse"
else
    echo "Unsupported Linux distribution. Please install dependencies manually."
    exit 1
fi

echo "Detected distro: $DISTRO"

# Install dependencies
if [ "$DISTRO" = "debian" ]; then
    sudo apt update
    sudo apt install -y build-essential cmake
elif [ "$DISTRO" = "arch" ]; then
    sudo pacman -Sy --needed --noconfirm base-devel cmake
elif [ "$DISTRO" = "fedora" ]; then
    sudo dnf install -y gcc gcc-c++ cmake make
elif [ "$DISTRO" = "centos" ]; then
    sudo yum groupinstall -y "Development Tools"
    sudo yum install -y cmake
elif [ "$DISTRO" = "opensuse" ]; then
    sudo zypper install -y cmake gcc gcc-c++ make
fi

## Build ##
if [ $1 == "clean" ]; then
    if [ "$(ls -A build)" ]; then 
        rm -r build/*
    fi
    if [ "$(ls -A bin)" ]; then 
        rm -r bin/* 
    fi
    mkdir bin/lib
fi

cmake -B build
cmake --build build #--config Release

cp scripts/run.sh bin/
cp scripts/cleanup.sh build/

echo "Succsessfully build Sentinel."
echo "Execute by running bin/run.sh."