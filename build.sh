#!/bin/bash
set -e

echo "==> Copying localversion..."
cp my-changes/localversion linux/

echo "==> Copying .config..."
cp my-changes/.config linux/

echo "==> Preparing config..."
make -C linux ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- olddefconfig

echo "==> Building kernel..."
make -C linux ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -j$(nproc) Image modules dtbs

echo "==> Installing modules..."
mkdir -p kernel_install
make -C linux ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- \
    INSTALL_MOD_PATH=../kernel_install modules_install \
    KERNELRELEASE=6.1.80-ti-arm64-r57

echo "==> Done! Files ready:"
echo "    Kernel:  linux/arch/arm64/boot/Image"
echo "    DTBs:    linux/arch/arm64/boot/dts/ti/*.dtb"
echo "    Modules: kernel_install/lib/modules/6.1.80-ti-arm64-r57"
