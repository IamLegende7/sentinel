#!/bin/bash

cd "$(dirname "$0")"
cd ..

package_dir=sentinel-linux

mkdir $package_dir

cp bin/sentinel $package_dir/
cp bin/run.sh $package_dir/
cp -r bin/lib $package_dir/

cp -r resources $package_dir/
cp -r data $package_dir/

tar -czvf $package_dir.tar.gz $package_dir

if [ $1 == "cleanup" ]; then
    rm -rf $package_dir
fi