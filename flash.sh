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

echo "==> Flashing U-Boot..."
sudo cp ~/beagley-workspace/u-boot/uboot-build/r5/tiboot3-j722s-hs-fs-evm.bin /media/yarona/BOOT/tiboot3.bin
sudo cp ~/beagley-workspace/u-boot/uboot-build/a53/tispl.bin /media/yarona/BOOT/tispl.bin
sudo cp ~/beagley-workspace/u-boot/uboot-build/a53/u-boot.img /media/yarona/BOOT/u-boot.img

echo "==> Syncing..."
sync

#echo "==> Ejecting SD card..."
#sudo eject /dev/sdh
echo "==> Ejecting SD card..."
SD_DEVICE=$(findmnt -n -o SOURCE --target /media/yarona/BOOT | head -1 | sed 's/[0-9]*$//')
if [ -n "$SD_DEVICE" ]; then
    sudo eject "$SD_DEVICE"
    echo "==> Ejected $SD_DEVICE"
else
    echo "==> WARNING: Could not find SD card device, please eject manually"
fi


echo "==> Done! You can now boot the BeagleY-AI"
