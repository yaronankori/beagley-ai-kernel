#!/bin/bash
set -e

SD_BOOT="/media/yarona/BOOT"
UBOOT_R5="$HOME/beagley-workspace/u-boot/uboot-build/r5/tiboot3-j722s-hs-fs-evm.bin"
UBOOT_A53_TISPL="$HOME/beagley-workspace/u-boot/uboot-build/a53/tispl.bin"
UBOOT_A53_IMG="$HOME/beagley-workspace/u-boot/uboot-build/a53/u-boot.img"

echo "==> Checking SD card is mounted..."
if [ ! -d "$SD_BOOT" ]; then
    echo "ERROR: $SD_BOOT not found. Mount the SD card BOOT partition first."
    exit 1
fi

echo "==> Flashing tiboot3.bin (R5 SPL)..."
cp "$UBOOT_R5" "$SD_BOOT/tiboot3.bin"

echo "==> Flashing tispl.bin (A53 SPL)..."
cp "$UBOOT_A53_TISPL" "$SD_BOOT/tispl.bin"

echo "==> Flashing u-boot.img (U-Boot proper)..."
cp "$UBOOT_A53_IMG" "$SD_BOOT/u-boot.img"

sync
echo "==> Done! U-Boot flashed successfully."
echo "==> You can now eject the SD card: sudo eject /dev/sdf"
