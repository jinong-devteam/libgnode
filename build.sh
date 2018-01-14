#!/bin/bash

# build libgnode 

case "$1" in
    clean)
        echo "Cleaning..."
        rm -rf release lib
        ;;

    *)
        echo "Building..."
        mkdir lib
        mkdir release
        cd release
        cmake ..
        make clean
        make
esac
exit 0
