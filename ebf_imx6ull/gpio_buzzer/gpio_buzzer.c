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

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dengxh");

int platform_driver_buzzer_probe(struct platform_device *pdev);
int platform_driver_buzzer_remove(struct platform_device *ppdev);

// device id & cdev & class & device

// file operations
static int buzzer_dev_open(struct inode *inodep, struct file *filep);
static int buzzer_dev_release(struct inode *inodep, struct file *filep);
static ssize_t buzzer_dev_read(struct file *f, char __user *buf, size_t len, loff_t *off);

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
};

int platform_driver_buzzer_probe(struct platform_device *pdev)
{
    printk(KERN_NOTICE "platform_driver_buzzer_probe!\r\n");

    return 0;
}

int platform_driver_buzzer_remove(struct platform_device *ppdev)
{
    printk(KERN_NOTICE "platform_driver_buzzer_remove!\r\n");

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
    int ret;
    
    return 0;
}

module_init(platform_driver_buzzer_init);
module_exit(platform_driver_buzzer_exit);