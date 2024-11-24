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
#include <linux/pinctrl/pinctrl.h>
#include <linux/i2c.h>
#include <linux/miscdevice.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dengxh");

// file operations
static int mpu6050_open(struct inode *inodep, struct file *filep);
static int mpu6050_release(struct inode *inodep, struct file *filep);
static ssize_t mpu6050_read(struct file *f, char __user *buf, size_t len, loff_t *off);
static ssize_t mpu6050_write(struct file *f, const char __user *buf, size_t len, loff_t *off);

struct file_operations mpu6050_fops = {
    .open = mpu6050_open,
    .release = mpu6050_release,
    .read = mpu6050_read,
    .write = mpu6050_write,
};

// misc device
struct miscdevice mpu6050_misc = {
    .fops = &mpu6050_fops,
    .minor = MISC_DYNAMIC_MINOR,
    .name = "fire-mpu6050"
};

// need mutex protect
// i2c_client
struct i2c_client *mpu6050_client;
// need mutex protect

// file ops
static int mpu6050_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_NOTICE "open\r\n");

    return 0;
}

static int mpu6050_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_NOTICE "release\r\n");

    return 0;
}

static ssize_t mpu6050_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_NOTICE "read\r\n");
    
    return 0;
}

static ssize_t mpu6050_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
    char buffer[256];
    int ret;

    printk(KERN_NOTICE "write\r\n");

    return len;
}

static int i2c_mpu6050_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    printk(KERN_NOTICE "i2c_mpu6050_probe, %s matched\r\n", id->name);

    mpu6050_client = client;
    misc_register(&mpu6050_misc);

    return 0;
}

static int i2c_mpu6050_remove(struct i2c_client *cleint)
{
    printk(KERN_NOTICE "i2c_mpu6050_remove\r\n");
    misc_deregister(&mpu6050_misc);

    return 0;
}

// register to i2c bus, not platform
struct of_device_id mpu6050_of_match[] = {
    // invensense,mpu6050 defined in overlay dts
    {.compatible = "invensense,mpu6050"},
    {}
};

struct i2c_device_id mpu6050_id[] = {
    {
        .name = "mpu6050",
        .driver_data = 0
    }
};

// 若i2c devices已经被注册, match则无效, 不会继续匹配
struct i2c_driver mpu6050_driver = {
    .probe = i2c_mpu6050_probe,
    .remove = i2c_mpu6050_remove,
    .driver = {
        .owner = THIS_MODULE,
        .of_match_table = mpu6050_of_match,
        .name = "fire-mpu6050"
    },
    .id_table = mpu6050_id
};



static int __init i2c_mpu6050_init(void)
{
    int ret;
    printk(KERN_NOTICE "i2c_mpu6050_init\r\n");

    ret = i2c_add_driver(&mpu6050_driver);
    if (ret < 0) {
        printk(KERN_ERR "i2c_add_driver failed\r\n");
        return -EINVAL;
    }

    return 0;
}

static void __exit i2c_mpu6050_exit(void)
{
    printk(KERN_NOTICE "i2c_mpu6050_exit\r\n");

    printk(KERN_DEBUG "i2c_del_driver begin\r\n");
    i2c_del_driver(&mpu6050_driver);
    printk(KERN_DEBUG "i2c_del_driver end\r\n");

}

module_init(i2c_mpu6050_init);
module_exit(i2c_mpu6050_exit);