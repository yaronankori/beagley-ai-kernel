#!/bin/bash
set -e

BOOT=/media/yarona/BOOT
ROOTFS=/media/yarona/rootfs
KERNEL_VERSION=6.1.80-ti-arm64-r57
WORKSPACE=~/beagley-workspace/linux
MODULES=~/beagley-workspace/kernel_install

echo "==> Checking SD card is mounted..."
if [ ! -d "$BOOT" ] || [ ! -d "$ROOTFS" ]; then
    echo "ERROR: SD card not mounted! Check /media/yarona/"
    exit 1
fi

echo "==> Flashing Kernel..."
cp $WORKSPACE/arch/arm64/boot/Image $BOOT/Image

echo "==> Flashing DTBs..."
sudo cp $WORKSPACE/arch/arm64/boot/dts/ti/*.dtb $BOOT/ti/

echo "==> Flashing Modules..."
sudo rm -rf $ROOTFS/lib/modules/$KERNEL_VERSION
sudo cp -a $MODULES/lib/modules/$KERNEL_VERSION $ROOTFS/lib/modules/

echo "==> Syncing..."
sync

echo "==> Ejecting SD card..."
sudo eject /dev/sdf

echo "==> Done! You can now boot the BeagleY-AI"
