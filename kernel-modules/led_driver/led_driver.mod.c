#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif


static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x92997ed8, "_printk" },
	{ 0x405bed5e, "device_destroy" },
	{ 0x2254988e, "class_destroy" },
	{ 0xca28fc18, "cdev_del" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xd2310814, "gpio_to_desc" },
	{ 0x94ae90ed, "gpiod_set_raw_value" },
	{ 0xfe990052, "gpio_free" },
	{ 0x47229b5c, "gpio_request" },
	{ 0x7d445f4d, "gpiod_direction_output_raw" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x15eff557, "cdev_init" },
	{ 0x1a5d55c3, "cdev_add" },
	{ 0xdcd0343f, "__class_create" },
	{ 0x4c1dc659, "device_create" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x12a4e128, "__arch_copy_from_user" },
	{ 0x8da6585d, "__stack_chk_fail" },
	{ 0x656e4a6e, "snprintf" },
	{ 0x6cbbfc54, "__arch_copy_to_user" },
	{ 0x7682ba4e, "__copy_overflow" },
	{ 0xd4621880, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "3E3C3C174655A7CE09A2BD8");
