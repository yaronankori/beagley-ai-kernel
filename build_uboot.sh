#!/bin/bash
set -e

cd ~/beagley-workspace/u-boot

echo "==> Building U-Boot R5..."
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- O=uboot-build/r5 BINMAN_INDIRS=~/beagley-workspace -j$(nproc)

echo "==> Building U-Boot A53..."
make ARCH=arm CROSS_COMPILE=aarch64-linux-gnu- O=uboot-build/a53 BINMAN_INDIRS=~/beagley-workspace BL31=$(pwd)/../prebuilt/bl31.bin TEE=$(pwd)/../prebuilt/tee-pager_v2.bin -j$(nproc)

echo "==> Done!"
