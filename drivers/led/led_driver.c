#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/gpio/consumer.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/of.h>

#define DRIVER_NAME     "led_driver"
#define DEVICE_NAME     "led"

static int major;
static struct class *led_class;
static struct device *led_device;
static struct gpio_desc *led_gpio;

/* ============================================================
 * FOPS - open
 * ============================================================ */
static int led_open(struct inode *inode, struct file *file)
{
    pr_info("%s: device opened\n", DRIVER_NAME);
    return 0;
}

/* ============================================================
 * FOPS - release
 * ============================================================ */
static int led_release(struct inode *inode, struct file *file)
{
    pr_info("%s: device closed\n", DRIVER_NAME);
    return 0;
}

/* ============================================================
 * FOPS - read
 * ============================================================ */
static ssize_t led_read(struct file *file, char __user *buf,
                        size_t count, loff_t *ppos)
{
    char state[3];
    int val;

    if (*ppos > 0)
        return 0;

    val = gpiod_get_value(led_gpio);
    snprintf(state, sizeof(state), "%d\n", val);

    if (copy_to_user(buf, state, 2))
        return -EFAULT;

    *ppos += 2;
    return 2;
}

/* ============================================================
 * FOPS - write
 * ============================================================ */
static ssize_t led_write(struct file *file, const char __user *buf,
                         size_t count, loff_t *ppos)
{
    char kbuf[2];

    if (count == 0)
        return -EINVAL;

    if (copy_from_user(kbuf, buf, 1))
        return -EFAULT;

    if (kbuf[0] == '1') {
        gpiod_set_value(led_gpio, 1);
        pr_info("%s: LED ON\n", DRIVER_NAME);
    } else if (kbuf[0] == '0') {
        gpiod_set_value(led_gpio, 0);
        pr_info("%s: LED OFF\n", DRIVER_NAME);
    } else {
        pr_warn("%s: invalid value, use 0 or 1\n", DRIVER_NAME);
        return -EINVAL;
    }

    return count;
}

/* ============================================================
 * File operations structure
 * ============================================================ */
static const struct file_operations led_fops = {
    .owner   = THIS_MODULE,
    .open    = led_open,
    .release = led_release,
    .read    = led_read,
    .write   = led_write,
};

/* ============================================================
 * Platform driver probe — called by kernel when
 * "beagley,led" is found in device tree
 * ============================================================ */
static int led_probe(struct platform_device *pdev)
{
    int ret;

    /* 1. Get GPIO from device tree */
    led_gpio = devm_gpiod_get(&pdev->dev, NULL, GPIOD_OUT_LOW);
    if (IS_ERR(led_gpio)) {
        dev_err(&pdev->dev, "failed to get GPIO from DT\n");
        return PTR_ERR(led_gpio);
    }

    /* 2. Register char device */
    major = register_chrdev(0, DEVICE_NAME, &led_fops);
    if (major < 0) {
        dev_err(&pdev->dev, "failed to register char device\n");
        return major;
    }

    /* 3. Create device class */
    led_class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(led_class)) {
        ret = PTR_ERR(led_class);
        goto err_chrdev;
    }

    /* 4. Create /dev/led */
    led_device = device_create(led_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(led_device)) {
        ret = PTR_ERR(led_device);
        goto err_class;
    }

    dev_info(&pdev->dev, "loaded! /dev/%s ready\n", DEVICE_NAME);
    return 0;

err_class:
    class_destroy(led_class);
err_chrdev:
    unregister_chrdev(major, DEVICE_NAME);
    return ret;
}

/* ============================================================
 * Platform driver remove — called when device is removed
 * ============================================================ */
static int led_remove(struct platform_device *pdev)
{
    device_destroy(led_class, MKDEV(major, 0));
    class_destroy(led_class);
    unregister_chrdev(major, DEVICE_NAME);
    gpiod_set_value(led_gpio, 0);
    dev_info(&pdev->dev, "unloaded\n");
    return 0;
}

/* ============================================================
 * Device tree match table
 * ============================================================ */
static const struct of_device_id led_of_match[] = {
    { .compatible = "beagley,led" },  /* matches DTS compatible string */
    { }
};
MODULE_DEVICE_TABLE(of, led_of_match);

/* ============================================================
 * Platform driver structure
 * ============================================================ */
static struct platform_driver led_platform_driver = {
    .probe  = led_probe,
    .remove = led_remove,
    .driver = {
        .name           = DRIVER_NAME,
        .of_match_table = led_of_match,
    },
};

module_platform_driver(led_platform_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("yarona");
MODULE_DESCRIPTION("BeagleY-AI LED platform driver using device tree");
