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

int platform_driver_button_probe(struct platform_device *pdev);
int platform_driver_button_remove(struct platform_device *ppdev);

#define DRV_NAME 

// device id & cdev & class & device
static dev_t button_dev_id;
static struct cdev button_cdev;
static struct class *button_class;
static struct device *button_device;

// dts node
static struct device_node *button_node;

// file operations
static int button_dev_open(struct inode *inodep, struct file *filep);
static int button_dev_release(struct inode *inodep, struct file *filep);
static ssize_t button_dev_read(struct file *f, char __user *buf, size_t len, loff_t *off);

// gpio subsystem
int button_gpio;

struct of_device_id match_table[] = {
    {
        .compatible = "ebf,button_key"
    },
    {    
    }
};

struct platform_driver platform_driver_button = {
    .probe = platform_driver_button_probe,
    .remove = platform_driver_button_remove,
    .driver = {
        .name = "ebf-imx6ull-key",
        .of_match_table = match_table
    }
};

struct file_operations button_dev_fops = {
    .open = button_dev_open,
    .release = button_dev_release,
    .read = button_dev_read,
};

int platform_driver_button_probe(struct platform_device *pdev)
{
    printk(KERN_NOTICE "probe!\r\n");

    // 找到dts里面的ebf_key节点
    button_node = of_find_node_by_path("/ebf_key");
    if (button_node == NULL) {
        printk(KERN_ERR "can not find 'ebf_key' in current dtb\r\n");
        return -EINVAL;
    }
    
    // 获取gpio句柄
    button_gpio = of_get_named_gpio(button_node, "key-gpio", 0);
    if (button_gpio < 0) {
        printk(KERN_ERR "of_get_named_gpio for rgb-red-gpio failed!\r\n");
        return -EINVAL;
    }
    gpio_direction_input(button_gpio);

    // dev_t
    alloc_chrdev_region(&button_dev_id, 0, 1, "ebf-key");

    // cdev
    cdev_init(&button_cdev, &button_dev_fops);
    cdev_add(&button_cdev, button_dev_id, 1);

    // class & device
    button_class = class_create(THIS_MODULE, "ebf-key");
    button_device = device_create(button_class, NULL, button_dev_id, NULL, "ebf-key");

    return 0;
}

int platform_driver_button_remove(struct platform_device *ppdev)
{
    printk(KERN_NOTICE "gpio_free begin\r\n");
    gpio_free(button_gpio);
    printk(KERN_NOTICE "gpio_free end\r\n");

    printk(KERN_NOTICE "cdev_del begin\r\n");
    cdev_del(&button_cdev);
    printk(KERN_NOTICE "cdev_del end\r\n");

    printk(KERN_NOTICE "device_destroy begin\r\n");
    device_destroy(button_class, button_dev_id);
    printk(KERN_NOTICE "device_destroy end\r\n");

    printk(KERN_NOTICE "class_destroy begin\r\n");
    class_destroy(button_class);
    printk(KERN_NOTICE "class_destroy end\r\n");

    printk(KERN_NOTICE "unregister_chrdev_region begin\r\n");
    unregister_chrdev_region(button_dev_id, 1);
    printk(KERN_NOTICE "unregister_chrdev_region end\r\n");

    return 0;
}

int platform_driver_button_init(void)
{
    printk(KERN_NOTICE "platform_driver_button_init\r\n");
    platform_driver_register(&platform_driver_button);

    return 0;
}

void platform_driver_button_exit(void)
{
    printk(KERN_NOTICE "platform_driver_button_exit\r\n");

    printk(KERN_NOTICE "platform_driver_unregister begin\r\n");
    platform_driver_unregister(&platform_driver_button);
    printk(KERN_NOTICE "platform_driver_unregister end\r\n");
}

// old functions
static int button_dev_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_NOTICE "open\r\n");

    return 0;
}

static int button_dev_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_NOTICE "release\r\n");

    return 0;
}

static ssize_t button_dev_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    int ret;
    static char buffer[12];
    static int buffer_len;
    static int gpio_value;

    printk(KERN_NOTICE "read\r\n");

    gpio_value = gpio_get_value(button_gpio);
    buffer_len = sprintf(buffer, "%d", gpio_value);
    ret = copy_to_user(buf, buffer, buffer_len);

    if (ret < 0) {
        printk(KERN_ERR "copy_to_user failed!\r\n");
        return -EFAULT;
    }
    return buffer_len;
}

module_init(platform_driver_button_init);
module_exit(platform_driver_button_exit);