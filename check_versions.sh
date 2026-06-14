# ================================================
# Kernel
# ================================================
strings /media/yarona/BOOT/Image | grep "Linux version"
# Output: Linux version 6.1.80-ti-arm64-r57 (build@beagle) ...
#                       ↑ version    ↑ tag

# ================================================
# U-Boot R5 SPL (tiboot3.bin)
# ================================================
strings /media/yarona/BOOT/tiboot3.bin | grep "U-Boot SPL"
# Output: U-Boot SPL 2026.01-g5c82fb688432 (Jun 11 2026 - 13:59:45)
#                    ↑version  ↑commit hash   ↑build date

# ================================================
# U-Boot A53 SPL (tispl.bin)
# ================================================
strings /media/yarona/BOOT/tispl.bin | grep "U-Boot"
# Output: U-Boot SPL 2026.01-g5c82fb688432

# ================================================
# U-Boot proper (u-boot.img)
# ================================================
strings /media/yarona/BOOT/u-boot.img | grep "U-Boot 20"
# Output: U-Boot 2026.01-g5c82fb688432 (Jun 11 2026 - 13:59:45)

# ================================================
# ATF (inside tispl.bin)
# ================================================
strings /media/yarona/BOOT/tispl.bin | grep "BL31\|TF-A"
# Output: BL31: v2.11.0(release):517b7f9

# ================================================
# OP-TEE (inside tispl.bin)
# ================================================
strings /media/yarona/BOOT/tispl.bin | grep "OP-TEE version"
# Output: OP-TEE version: 4.2.0

# ================================================
# DTB - board model
# ================================================
strings /media/yarona/BOOT/ti/k3-am67a-beagley-ai.dtb | grep "BeagleY\|model"
# Output: BeagleBoard.org BeagleY-AI

# ================================================
# Modules
# ================================================
ls /media/yarona/rootfs/lib/modules/
# Output: 6.1.80-ti-arm64-r57

# ================================================
# All in one script
# ================================================
echo "=== Kernel ===" && strings /media/yarona/BOOT/Image | grep "Linux version"
echo "=== tiboot3 ===" && strings /media/yarona/BOOT/tiboot3.bin | grep "U-Boot SPL"
echo "=== tispl ===" && strings /media/yarona/BOOT/tispl.bin | grep "U-Boot SPL"
echo "=== u-boot ===" && strings /media/yarona/BOOT/u-boot.img | grep "U-Boot 20"
echo "=== ATF ===" && strings /media/yarona/BOOT/tispl.bin | grep "BL31"
echo "=== OP-TEE ===" && strings /media/yarona/BOOT/tispl.bin | grep "OP-TEE version"
echo "=== Modules ===" && ls /media/yarona/rootfs/lib/modules/
