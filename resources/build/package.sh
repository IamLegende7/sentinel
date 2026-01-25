#!/bin/bash

cd "$(dirname "$0")"
cd ..

package_dir=sentinel-linux
do_cleanup=false
delete_source=true

OPTIONS=$(getopt -o c,n: --long cleanup,name:,delete-source -- "$@")
eval set -- "$OPTIONS"
while true; do
    case "$1" in
        --cleanup)
            do_cleanup=true
            shift
            ;;
        -n|--name)
            package_dir="$2"
            shift 2
            ;;
        --delete-source)
            delete_source=true
            shift
            ;;
        --) 
            shift
            break
            ;;
        *) 
            echo "Usage: $0 [-n | --name package_name] [--cleanup] [--delete-source]"
            exit 1
            ;;
    esac
done

if [ "$(ls -A "$package_dir")" ]; then 
  rm -r "$package_dir"
fi

mkdir "$package_dir"

cp bin/sentinel "$package_dir/"
cp bin/run.sh "$package_dir/"
cp -r bin/lib "$package_dir/"

cp -r resources "$package_dir/"
rm -rf "$package_dir/resources/build"
cp -r data "$package_dir/"

cp LICENSE "$package_dir/"
cp resources/build/RELEASE_README.md "$package_dir/README.md"

if [ "$(ls -A "$package_dir.tar.gz")" ]; then 
    rm "$package_dir.tar.gz"
fi

tar -czvf "$package_dir.tar.gz" "$package_dir"

if [ "$do_cleanup" = true ]; then
    echo "Cleaning up.."
    rm -rf "$package_dir"
fi

if [ "$delete_source" = true ]; then
    echo "Deleting compiled source.."
    rm -rf bin/*
fi