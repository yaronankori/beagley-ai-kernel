#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/uaccess.h>

#define DRIVER_NAME     "led_driver"
#define DEVICE_NAME     "led"
#define GPIO_LED        38

static int major;
static struct class *led_class;
static struct device *led_device;

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
 * FOPS - read  (returns current LED state: "0\n" or "1\n")
 * ============================================================ */
static ssize_t led_read(struct file *file, char __user *buf,
                        size_t count, loff_t *ppos)
{
    char state[3];
    int val;

    if (*ppos > 0)
        return 0;

    val = gpio_get_value(GPIO_LED);
    snprintf(state, sizeof(state), "%d\n", val);

    if (copy_to_user(buf, state, 2))
        return -EFAULT;

    *ppos += 2;
    return 2;
}

/* ============================================================
 * FOPS - write  (write "1" to turn ON, "0" to turn OFF)
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
        gpio_set_value(GPIO_LED, 1);
        pr_info("%s: LED ON\n", DRIVER_NAME);
    } else if (kbuf[0] == '0') {
        gpio_set_value(GPIO_LED, 0);
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
 * Module init
 * ============================================================ */
static int __init led_init(void)
{
    int ret;

    /* 1. Request GPIO */
    ret = gpio_request(GPIO_LED, DRIVER_NAME);
    if (ret) {
        pr_err("%s: failed to request GPIO %d\n", DRIVER_NAME, GPIO_LED);
        return ret;
    }

    /* 2. Set GPIO as output, initial value 0 (OFF) */
    ret = gpio_direction_output(GPIO_LED, 0);
    if (ret) {
        pr_err("%s: failed to set GPIO direction\n", DRIVER_NAME);
        goto err_gpio;
    }

    /* 3. Register char device */
    major = register_chrdev(0, DEVICE_NAME, &led_fops);
    if (major < 0) {
        pr_err("%s: failed to register char device\n", DRIVER_NAME);
        ret = major;
        goto err_gpio;
    }

    /* 4. Create device class */
    led_class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(led_class)) {
        pr_err("%s: failed to create class\n", DRIVER_NAME);
        ret = PTR_ERR(led_class);
        goto err_chrdev;
    }

    /* 5. Create device node /dev/led */
    led_device = device_create(led_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(led_device)) {
        pr_err("%s: failed to create device\n", DRIVER_NAME);
        ret = PTR_ERR(led_device);
        goto err_class;
    }

    pr_info("%s: loaded! GPIO=%d major=%d /dev/%s ready\n",
            DRIVER_NAME, GPIO_LED, major, DEVICE_NAME);
    return 0;

err_class:
    class_destroy(led_class);
err_chrdev:
    unregister_chrdev(major, DEVICE_NAME);
err_gpio:
    gpio_free(GPIO_LED);
    return ret;
}

/* ============================================================
 * Module exit
 * ============================================================ */
static void __exit led_exit(void)
{
    device_destroy(led_class, MKDEV(major, 0));
    class_destroy(led_class);
    unregister_chrdev(major, DEVICE_NAME);
    gpio_set_value(GPIO_LED, 0);
    gpio_free(GPIO_LED);
    pr_info("%s: unloaded\n", DRIVER_NAME);
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("yarona");
MODULE_DESCRIPTION("LED GPIO driver for BeagleY-AI pin 7 (GPIO4)");
