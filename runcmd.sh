#!/bin/bash

docker run --rm -v "$(pwd):/app" -w /app smatyukevich/raspberry-pi-os-builder aarch64-linux-gnu-readelf -a build/drivers/v3d_test_s.o


