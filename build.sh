#!/bin/bash
set -e

cd "$(dirname "$0")"

do_clean_build=false
auto_package=false
install_build_dependencies=true
do_cleanup=false
show_warning=true
compile_shadercross=true
compile_shaders=false

OPTIONS=$(getopt -o c,p,d,h,s --long clean,auto-package,no-install-deps,cleanup,help,no-show-warning,no-shadercross,offline-shaders -- "$@")
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
    --no-shadercross)
      compile_shadercross=false
      shift
      ;;
    -s|--offline-shaders)
      compile_shaders=true
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
      echo "[-d|--no-install-deps] Don't install dependencies automatically"
      echo "[--cleanup]            Cleanup build files"
      echo "[--no-show-warning]    Don't show the 'Has only been tested on arch' warning"
      echo "[--no-shadercross]     Don't compile SDL_shadercross. Drasticly shortens the compile times, but the shadercross will need to be compiled at least once for SDL_gpu to work"
      echo "[-s|--offline-shaders] Precompile shaders"
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
    echo "THIS SCRIPT HAS ONLY BEEN TESTED ON ARCH LINUX. PROCEED WITH CAUTION! [Press <enter> to continue]"
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
    else
        echo "Unsupported Linux distro. Please install dependencies manually."
        exit 1
    fi

    echo "Detected distro: $DISTRO"

    # Install dependencies
    if [ "$DISTRO" = "debian" ]; then
        sudo apt update
        sudo apt install -y build-essential cmake libvkd3d-utils1
    elif [ "$DISTRO" = "arch" ]; then
        sudo pacman -Sy --needed --noconfirm base-devel cmake vkd3d
    fi
fi



if [ "$do_clean_build" = true ]; then
    if [ "$(ls -A build)" ]; then 
        rm -r build/*
    fi
    if [ "$(ls -A bin)" ]; then 
        rm -r bin/* 
    fi
fi
mkdir -p bin
mkdir -p bin/lib

## SDL_Shadercross ##
if [ "$compile_shadercross" = true ]; then
    if [ "$do_clean_build" = true ]; then
        if [ "$(ls -A build/shader_build)" ]; then
            rm -r build/shader_build
        fi
    fi
    mkdir -p build/shader_build

    pushd lib/SDL_shadercross
    cmake -B ../../build/shader_build -DSDLSHADERCROSS_VENDORED=ON
    cmake --build ../../build/shader_build
    popd
fi

## Build ##
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

## Shaders ##
if [ "$compile_shaders" = true ]; then
    echo "Compiling shaders.."
    mkdir -p bin/shaders/SPIRV
    mkdir -p bin/shaders/MSL
    mkdir -p bin/shaders/DXIL
    pushd resources/shaders
    if [ -f "../../build/shader_build/shadercross" ]; then
        for filename in *.hlsl; do
            if [ -f "$filename" ]; then
                ../../build/shader_build/shadercross "$filename" -o "../../bin/shaders/SPIRV/${filename/.hlsl/.spv}"
                ../../build/shader_build/shadercross "$filename" -o "../../bin/shaders/MSL/${filename/.hlsl/.msl}"
                ../../build/shader_build/shadercross "$filename" -o "../../bin/shaders/DXIL/${filename/.hlsl/.dxil}"
            fi
        done
    else
        echo "ERORR: SDL_shadercross isn't build!"
        exit 1
    fi
    popd
    echo "Shaders successfully compiled!"
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

echo "All done. Enjoy!"