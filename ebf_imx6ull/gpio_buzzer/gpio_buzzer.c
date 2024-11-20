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
#include <linux/atomic.h>
#include <linux/miscdevice.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dengxh");

#define BUZZER_MINOR 185

// platform driver
int platform_driver_buzzer_probe(struct platform_device *pdev);
int platform_driver_buzzer_remove(struct platform_device *ppdev);

// buzzer gpio
int buzzer_gpio;

// file operations
static int buzzer_dev_open(struct inode *inodep, struct file *filep);
static int buzzer_dev_release(struct inode *inodep, struct file *filep);
static ssize_t buzzer_dev_read(struct file *f, char __user *buf, size_t len, loff_t *off);
static ssize_t buzzer_dev_write(struct file *fp, const char __user *buf, size_t len, loff_t *off);

struct of_device_id match_table[] = {
    {
        .compatible = "ebf,buzzer"
    },
    {    
    }
};

struct platform_driver platform_driver_buzzer = {
    .probe = platform_driver_buzzer_probe,
    .remove = platform_driver_buzzer_remove,
    .driver = {
        .name = "ebf-buzzer",
        .of_match_table = match_table
    }
};

struct file_operations buzzer_dev_fops = {
    .open = buzzer_dev_open,
    .release = buzzer_dev_release,
    .read = buzzer_dev_read,
    .write = buzzer_dev_write,
};

static struct miscdevice buzzer_misc = {
    .fops = &buzzer_dev_fops,
    .minor = BUZZER_MINOR,
    .name = "ebf_buzzer"
};

int platform_driver_buzzer_probe(struct platform_device *pdev)
{
    struct device_node *buzzer_node;

    printk(KERN_NOTICE "platform_driver_buzzer_probe!\r\n");
    misc_deregister(&buzzer_misc);

    buzzer_node = of_find_node_by_path("/ebf_buzzer");
    if (buzzer_node == NULL) {
        printk(KERN_ERR "find node 'buzzer_node' failed\r\n");
        return -EINVAL;
    }
    buzzer_gpio = of_get_named_gpio(buzzer_node, "gpio_buzzer", 0);
    if (buzzer_gpio < 0) {
        printk(KERN_ERR "of_get_named_gpio failed\r\n");
        return -EINVAL;
    }

    gpio_direction_output(buzzer_gpio, 0);
    gpio_set_value(buzzer_gpio, 0);

    return 0;
}

int platform_driver_buzzer_remove(struct platform_device *ppdev)
{
    printk(KERN_NOTICE "platform_driver_buzzer_remove!\r\n");
    misc_deregister(&buzzer_misc);

    gpio_free(buzzer_gpio);

    return 0;
}

int platform_driver_buzzer_init(void)
{
    printk(KERN_NOTICE "platform_driver_buzzer_init\r\n");
    platform_driver_register(&platform_driver_buzzer);

    return 0;
}

void platform_driver_buzzer_exit(void)
{
    printk(KERN_NOTICE "platform_driver_buzzer_exit\r\n");

    platform_driver_unregister(&platform_driver_buzzer);
}

static int buzzer_dev_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_NOTICE "open\r\n");

    return 0;
}

static int buzzer_dev_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_NOTICE "release\r\n");

    return 0;
}

static ssize_t buzzer_dev_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    // int ret;
    
    return 0;
}

static ssize_t buzzer_dev_write(struct file *fp, const char __user *buf, size_t len, loff_t *off)
{
    char buffer[16];
    int ret;
    printk(KERN_NOTICE "write\r\n");

    ret = copy_from_user(buffer, buf, len);
    if (ret < 0) {
        printk(KERN_ERR "copy_from_user failed!\r\n");
        return -EINVAL;
    }
    if (buffer[0] == '1') {
        gpio_set_value(buzzer_gpio, 1);
        printk(KERN_NOTICE "buzzer on!\r\n");
    } else {
        gpio_set_value(buzzer_gpio, 0);
        printk(KERN_NOTICE "buzzer off!\r\n");
    }

    return len;
}


module_init(platform_driver_buzzer_init);
module_exit(platform_driver_buzzer_exit);