#!/bin/bash

docker run --rm -v "$(pwd):/app" -w /app ubuntu make TARGET_PLATFORM=${1:-RPI3}

cp -f kernel8.img /Volumes/boot/kernel8.img
sync
cp -f config.txt /Volumes/boot/config.txt
sync
cp -f armstub-new.bin /Volumes/boot/armstub-new.bin
sync
