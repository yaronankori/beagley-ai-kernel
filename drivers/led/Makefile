obj-m += led_driver.o

KERNEL_DIR ?= $(HOME)/beagley-workspace/linux
ARCH := arm64
CROSS_COMPILE := aarch64-linux-gnu-

all:
	make -C $(KERNEL_DIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

clean:
	make -C $(KERNEL_DIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) clean
