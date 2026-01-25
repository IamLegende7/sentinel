#!/bin/bash
set -e

cd "$(dirname "$0")"

do_clean_build=false
auto_package=false
install_build_dependencies=true
do_cleanup=false
show_warning=true

OPTIONS=$(getopt -o c,p,d,h --long clean,auto-package,no-install-deps,cleanup,help,no-show-warning -- "$@")
eval set -- "$OPTIONS"
while true; do
  case "$1" in
    -c|--clean)
      do_clean_build=true
      shift
      ;;
    -p|--auto-package)
      auto_package=true
      shift
      ;;
    -d|--no-install-deps)
      install_build_dependencies=false
      shift
      ;;
    --cleanup)
      do_cleanup=true
      shift
      ;;
    --no-show-warning)
      show_warning=false
      shift
      ;;
    --)
      shift
      break
      ;;
    -h|--help)
      echo "Usage: $0 [-c|--clean] [-p|--auto-package] [-d|--no-install-deps] [--cleanup]"
      echo ""
      echo "[-c|--clean]           Make a clean build (Remove all old build files & binarys)"
      echo "[-p|--auto-package]    Package the compiled code using package.sh into a .tar.gz with only the importand files"
      echo "[-d|--no-install-deps] (**Not Recomended!**) Don't install dependencies automatically"
      echo "[--cleanup]            Cleanup build files"
      exit 1
      ;;
    *) 
      echo "Usage: $0 [-c|--clean] [-p|--auto-package] [-d|--no-install-deps] [--cleanup]"
      echo Try '$0 --help' for more information.
      exit 1
      ;;
  esac
done

if [ "$show_warning" = true ]; then
    echo "THIS SCRIPT HAS ONLY BEEN TESTED ON ARCH LINUX. PROCEED WITH CAUTION!"
    read
fi

# Detect distro
if [[ "$OSTYPE" != "linux-gnu"* ]]; then
    echo "This bash script is only for Linux. Use the build script for your OS ($OSTYPE)"
    exit 1
fi

if [ "$install_build_dependencies" = true ]; then
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
fi

## Build ##
if [ "$do_clean_build" = true ]; then
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

cp resources/build/run.sh bin/
cp resources/build/package.sh bin/
cp resources/build/RELEASE_README.md bin/README.md

cp resources/build/cleanup.sh build/

echo "Succsessfully build Sentinel."
if [ "$auto_package" = false ]; then
    echo "Execute by running bin/run.sh."
fi


## Packaging
if [ "$auto_package" = true ]; then
    echo "Packaging sentinel.."
    ./bin/package.sh --cleanup
    echo "Done!"
fi

## Cleanup
if [ "$do_cleanup" = true ]; then
    echo "Cleaning up build/*"
    ./build/cleanup.sh
    echo "Done!"
fi