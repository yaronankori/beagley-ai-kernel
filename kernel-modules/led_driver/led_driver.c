// SPDX-License-Identifier: GPL-2.0
/*
 * led_driver.c - Simple LED character device
 * 
 * Creates /dev/myled that controls GPIO 439 (Pin 7 on BeagleY-AI)
 * Usage:
 *   echo 1 > /dev/myled  → LED ON
 *   echo 0 > /dev/myled  → LED OFF
 *   cat /dev/myled       → show current state
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>

#define DEVICE_NAME "myled"
#define CLASS_NAME  "myled_class"
#define GPIO_NUM    439    /* GPIO 4 on header pin 7 */

static dev_t dev_num;
static struct cdev myled_cdev;
static struct class *myled_class;
static int led_state = 0;

/* Called when userspace opens /dev/myled */
static int myled_open(struct inode *inode, struct file *filp)
{
    pr_info("myled: device opened\n");
    return 0;
}

/* Called when userspace closes /dev/myled */
static int myled_release(struct inode *inode, struct file *filp)
{
    pr_info("myled: device closed\n");
    return 0;
}

/* Called when userspace writes to /dev/myled (echo 1 > /dev/myled) */
static ssize_t myled_write(struct file *filp, const char __user *buf,
                            size_t count, loff_t *offset)
{
    char kbuf[4] = {0};
    size_t len = min(count, sizeof(kbuf) - 1);

    if (copy_from_user(kbuf, buf, len))
        return -EFAULT;

    if (kbuf[0] == '1') {
        gpio_set_value(GPIO_NUM, 1);
        led_state = 1;
        pr_info("myled: LED ON\n");
    } else if (kbuf[0] == '0') {
        gpio_set_value(GPIO_NUM, 0);
        led_state = 0;
        pr_info("myled: LED OFF\n");
    } else {
        pr_warn("myled: invalid value (use 0 or 1)\n");
    }

    return count;
}

/* Called when userspace reads /dev/myled (cat /dev/myled) */
static ssize_t myled_read(struct file *filp, char __user *buf,
                           size_t count, loff_t *offset)
{
    char kbuf[4];
    int len;

    if (*offset > 0)   /* only return once, not in a loop */
        return 0;

    len = snprintf(kbuf, sizeof(kbuf), "%d\n", led_state);

    if (copy_to_user(buf, kbuf, len))
        return -EFAULT;

    *offset += len;
    return len;
}

/* File operations table - tells kernel what functions to call */
static const struct file_operations myled_fops = {
    .owner   = THIS_MODULE,
    .open    = myled_open,
    .release = myled_release,
    .write   = myled_write,
    .read    = myled_read,
};

/* Called when the module is loaded (insmod) */
static int __init myled_init(void)
{
    int ret;

    pr_info("myled: initializing driver\n");

    /* Step 1: Request the GPIO from the kernel */
    ret = gpio_request(GPIO_NUM, "myled-gpio");
    if (ret) {
        pr_err("myled: failed to request GPIO %d\n", GPIO_NUM);
        return ret;
    }

    /* Step 2: Configure GPIO as output, starting LOW */
    ret = gpio_direction_output(GPIO_NUM, 0);
    if (ret) {
        pr_err("myled: failed to set GPIO direction\n");
        gpio_free(GPIO_NUM);
        return ret;
    }

    /* Step 3: Allocate a device number */
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        pr_err("myled: failed to allocate device number\n");
        gpio_free(GPIO_NUM);
        return ret;
    }

    /* Step 4: Initialize the character device */
    cdev_init(&myled_cdev, &myled_fops);
    myled_cdev.owner = THIS_MODULE;

    /* Step 5: Add the character device to the system */
    ret = cdev_add(&myled_cdev, dev_num, 1);
    if (ret) {
        pr_err("myled: failed to add cdev\n");
        unregister_chrdev_region(dev_num, 1);
        gpio_free(GPIO_NUM);
        return ret;
    }

    /* Step 6: Create device class (for /dev auto-population) */
    myled_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(myled_class)) {
        pr_err("myled: failed to create class\n");
        cdev_del(&myled_cdev);
        unregister_chrdev_region(dev_num, 1);
        gpio_free(GPIO_NUM);
        return PTR_ERR(myled_class);
    }

    /* Step 7: Create device node in /dev/ */
    device_create(myled_class, NULL, dev_num, NULL, DEVICE_NAME);

    pr_info("myled: driver loaded! /dev/%s created (major=%d)\n",
            DEVICE_NAME, MAJOR(dev_num));

    return 0;
}

/* Called when the module is unloaded (rmmod) */
static void __exit myled_exit(void)
{
    pr_info("myled: unloading driver\n");

    device_destroy(myled_class, dev_num);
    class_destroy(myled_class);
    cdev_del(&myled_cdev);
    unregister_chrdev_region(dev_num, 1);

    /* Turn off LED and release GPIO */
    gpio_set_value(GPIO_NUM, 0);
    gpio_free(GPIO_NUM);

    pr_info("myled: driver unloaded\n");
}

module_init(myled_init);
module_exit(myled_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yaron Ankori");
MODULE_DESCRIPTION("Simple LED character device for BeagleY-AI");
MODULE_VERSION("0.1");
