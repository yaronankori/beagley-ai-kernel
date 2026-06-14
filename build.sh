#!/bin/bash
set -e

echo "==> Copying localversion..."
cp my-changes/localversion linux/

echo "==> Fixing scmversion..."
echo "" > linux/.scmversion

echo "==> Setting up driver in kernel tree..."
# ... all driver setup here ...
echo "==> Setting up driver in kernel tree..."
mkdir -p linux/drivers/led_beagle
cp my-changes/drivers/led/led_driver.c linux/drivers/led_beagle/


# Create Kconfig
cat > linux/drivers/led_beagle/Kconfig << 'EOF'
config LED_BEAGLE
    bool "BeagleY-AI LED GPIO driver"
    depends on GPIOLIB
    default y
    help
      LED driver for BeagleY-AI pin 7 (GPIO4)
EOF

# Create Makefile
printf 'obj-$(CONFIG_LED_BEAGLE) += led_driver.o\n' > linux/drivers/led_beagle/Makefile

echo "==> Patching kernel Kconfig..."
grep -q "led_beagle" linux/drivers/Kconfig || \
    sed -i '/endmenu/i source "drivers/led_beagle/Kconfig"' linux/drivers/Kconfig

echo "==> Patching kernel Makefile..."
grep -q "led_beagle" linux/drivers/Makefile || \
    echo 'obj-$(CONFIG_LED_BEAGLE) += led_beagle/' >> linux/drivers/Makefile



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

# ================================================
# U-Boot build
# ================================================
echo "==> Setting up U-Boot firmware structure..."
mkdir -p ti-sysfw ti-dm
cp -r ti-linux-firmware/ti-sysfw/* ti-sysfw/
cp -r ti-linux-firmware/ti-dm/j722s ti-dm/

echo "==> Building U-Boot R5..."
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- \
    am67a_beagley_ai_r5_defconfig O=uboot-build/r5 \
    -C u-boot
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- \
    O=uboot-build/r5 \
    BINMAN_INDIRS=$(pwd) \
    -C u-boot -j$(nproc)

echo "==> Building U-Boot A53..."
make ARCH=arm CROSS_COMPILE=aarch64-linux-gnu- \
    am67a_beagley_ai_a53_defconfig O=uboot-build/a53 \
    -C u-boot
make ARCH=arm CROSS_COMPILE=aarch64-linux-gnu- \
    O=uboot-build/a53 \
    BINMAN_INDIRS=$(pwd) \
    BL31=prebuilt/bl31.bin \
    TEE=prebuilt/tee-pager_v2.bin \
    -C u-boot -j$(nproc)

echo "==> Done! Files ready:"
echo "    Kernel:   linux/arch/arm64/boot/Image"
echo "    DTBs:     linux/arch/arm64/boot/dts/ti/*.dtb"
echo "    Modules:  kernel_install/lib/modules/6.1.80-ti-arm64-r57"
echo "    tiboot3:  uboot-build/r5/tiboot3-j722s-hs-fs-evm.bin"
echo "    tispl:    uboot-build/a53/tispl.bin"
echo "    u-boot:   uboot-build/a53/u-boot.img"
