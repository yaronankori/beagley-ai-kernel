// SPDX-License-Identifier: GPL-2.0

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int __init hello_init(void)
{
    pr_info("Hello World! Kernel module loaded from BeagleY-AI\n");
    pr_info("Yaron's first kernel module is alive!\n");
    return 0;
}

static void __exit hello_exit(void)
{
    pr_info("Goodbye from Yaron's kernel module!\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yaron Ankori");
MODULE_DESCRIPTION("First kernel module - hello world");
MODULE_VERSION("0.1");
