#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/file.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>

static int misc_chrdev_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_NOTICE "open\r\n");

    return 0;
}

static int misc_chrdev_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_NOTICE "release\r\n");

    return 0;
}

static ssize_t misc_chrdev_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_NOTICE "read\r\n");
    
    return 0;
}

static ssize_t misc_chrdev_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_NOTICE "write\r\n");

    return len;
}

struct file_operations misc_chrdev_fops = {
    .open = misc_chrdev_open,
    .release = misc_chrdev_release,
    .read = misc_chrdev_read,
    .write = misc_chrdev_write
};

struct miscdevice misc_chrdev = {
    .fops = &misc_chrdev_fops,
    .name = "misc_chrdev",
    .minor = MISC_DYNAMIC_MINOR
};

static int __init misc_chrdev_init(void)
{
    int ret;
    printk(KERN_NOTICE "init\r\n");

    ret = misc_register(&misc_chrdev);
    if (ret < 0) {
        printk(KERN_ERR "misc_register failed\r\n");
        return -EINVAL;
    } else {
        printk(KERN_NOTICE "misc_register success\r\n");

    }
    
    return 0;
}

static void __exit misc_chrdev_exit(void)
{
    printk(KERN_NOTICE "exit\r\n");

    misc_deregister(&misc_chrdev);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dengxh");

module_init(misc_chrdev_init);
module_exit(misc_chrdev_exit);