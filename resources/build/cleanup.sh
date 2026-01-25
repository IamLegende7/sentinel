#!/bin/bash

cd "$(dirname "$0")"

if [[ "$PWD" == */resources/build/ ]]; then
    echo "DO NOt RUN THIS SCRIPT OUTSIDE OF 'build/'!"
    echo "Abording..."
else
    rm -r ../build/*
fi