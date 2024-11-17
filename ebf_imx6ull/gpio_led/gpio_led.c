#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dengxh");

int platform_driver_led_probe(struct platform_device *pdev);
int platform_driver_led_remove(struct platform_device *ppdev);

static dev_t led_dev_id;
static struct cdev led_cdev;
static struct class *led_class;
static struct device *led_device;

// dts node
static struct device_node *led_node;

// file operations
static int led_dev_open(struct inode *inodep, struct file *filep);
static int led_dev_release(struct inode *inodep, struct file *filep);
static ssize_t led_dev_read(struct file *f, char __user *buf, size_t len, loff_t *off);
static ssize_t led_dev_write(struct file *f, const char __user *buf, size_t len, loff_t *off);
static long led_dev_ioctl(struct file *filep, unsigned int cmd, unsigned long para);

// gpio subsystem
int red_led_gpio;

struct of_device_id match_table[] = {
    {
        .compatible = "ebf,rgb_led"
    },
    {
        
    }
};

struct platform_driver led_driver = {
    .probe = platform_driver_led_probe,
    .remove = platform_driver_led_remove,
    .driver.name = "ebf-imx6ull-led",
    .driver.of_match_table = match_table
};

enum LED_DEV_IOCTL_CMD {
    LED_ON_CMD = 1,
    LED_OFF_CMD = 2,
    LED_TOGGLE_CMD = 3
};

struct file_operations led_dev_fops = {
    .open = led_dev_open,
    .release = led_dev_release,
    .read = led_dev_read,
    .write = led_dev_write,
    .compat_ioctl = led_dev_ioctl,
    .unlocked_ioctl = led_dev_ioctl
};

int platform_driver_led_probe(struct platform_device *pdev)
{
    printk(KERN_NOTICE "probe!\r\n");

    // 找到dts里面的rgb_led节点
    led_node = of_find_node_by_path("/rgb_led");
    if (led_node == NULL) {
        printk(KERN_ERR "can not find rgb_led in current dtb\r\n");
        return -EINVAL;
    }
    
    // 获取gpio句柄
    red_led_gpio = of_get_named_gpio(led_node, "rgb-red-gpio", 0);
    if (red_led_gpio < 0) {
        printk(KERN_ERR "of_get_named_gpio for rgb-red-gpio failed!\r\n");
        return -EINVAL;
    }
    // 1: 高电平 先不亮
    gpio_direction_output(red_led_gpio, 1);

    // dev_t
    alloc_chrdev_region(&led_dev_id, 0, 1, "ebf-led");

    // cdev
    cdev_init(&led_cdev, &led_dev_fops);
    cdev_add(&led_cdev, led_dev_id, 1);

    // class & device
    led_class = class_create(THIS_MODULE, "ebf-led");
    led_device = device_create(led_class, NULL, led_dev_id, NULL, "ebf-led");

    return 0;
}

int platform_driver_led_remove(struct platform_device *ppdev)
{
    printk(KERN_NOTICE "gpio_free begin\r\n");
    gpio_free(red_led_gpio);
    printk(KERN_NOTICE "gpio_free end\r\n");

    printk(KERN_NOTICE "cdev_del begin\r\n");
    cdev_del(&led_cdev);
    printk(KERN_NOTICE "cdev_del end\r\n");

    printk(KERN_NOTICE "device_destroy begin\r\n");
    device_destroy(led_class, led_dev_id);
    printk(KERN_NOTICE "device_destroy end\r\n");

    printk(KERN_NOTICE "class_destroy begin\r\n");
    class_destroy(led_class);
    printk(KERN_NOTICE "class_destroy end\r\n");

    printk(KERN_NOTICE "unregister_chrdev_region begin\r\n");
    unregister_chrdev_region(led_dev_id, 1);
    printk(KERN_NOTICE "unregister_chrdev_region end\r\n");

    return 0;
}

int platform_driver_led_init(void)
{
    printk(KERN_NOTICE "platform_driver_led_init\r\n");
    platform_driver_register(&led_driver);

    return 0;
}

void platform_driver_led_exit(void)
{
    printk(KERN_NOTICE "platform_driver_led_exit\r\n");

    printk(KERN_NOTICE "platform_driver_unregister begin\r\n");
    platform_driver_unregister(&led_driver);
    printk(KERN_NOTICE "platform_driver_unregister end\r\n");
}

// old functions
static int led_dev_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_NOTICE "open\r\n");

    return 0;
}

static int led_dev_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_NOTICE "release\r\n");

    return 0;
}

static ssize_t led_dev_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    char msg_to_user[] = "hello wrorld\r\n";
    int ret;

    printk(KERN_NOTICE "read\r\n");

    ret = copy_to_user(buf, msg_to_user, sizeof(msg_to_user));
    if (ret < 0) {
        printk(KERN_ERR "copy_to_user failed!\r\n");
        return EFAULT;
    }
    
    return sizeof(msg_to_user);
}

static ssize_t led_dev_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
    char buffer[256];
    int ret;

    printk(KERN_NOTICE "write\r\n");

    ret = copy_from_user(buffer, buf, len);
    if (ret < 0) {
        printk(KERN_ERR "copy_from_user failed!\r\n");
        return EFAULT;
    }
    printk(KERN_NOTICE "read %d bytes from userspace: [%s]\r\n", (int)len, buffer);

    if (strlen(buffer) > 1) {
        ret = buffer[0] - '0';
    }

    if (ret == 1) {
        gpio_set_value(red_led_gpio, 0);
    } else {
        gpio_set_value(red_led_gpio, 1);
    }


    return len;
}

static long led_dev_ioctl(struct file *filep, unsigned int cmd, unsigned long para)
{
    int sw = -1;
    
    printk(KERN_NOTICE "ioctl\r\n");

    switch (cmd)
    {
    case LED_ON_CMD:
        printk(KERN_NOTICE "LED_ON_CMD, para: %d\r\n", (int)para);
        sw = 1;
        break;
    case LED_OFF_CMD:
        printk(KERN_NOTICE "LED_OFF_CMD, para: %d\r\n", (int)para);
        sw = 0;
        break;
    case LED_TOGGLE_CMD:
        printk(KERN_NOTICE "LED_TOGGLE_CMD, para: %d\r\n", (int)para);
        break; 
    default:
        printk(KERN_NOTICE "invalid cmd, para: %d\r\n", (int)para);
        return EINVAL;
    }

    if (sw == 1) {
        gpio_set_value(red_led_gpio, 0);
    } else {
        gpio_set_value(red_led_gpio, 1);
    }

    return 0;
}

module_init(platform_driver_led_init);
module_exit(platform_driver_led_exit);