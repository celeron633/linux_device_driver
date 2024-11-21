#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/file.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/module.h>

#define CHRDEVBASE_MAJOR 200
#define CHRDEVBASE_NAME "chrdevbase"

static int chrdevbase_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_NOTICE "open\r\n");

    return 0;
}

static int chrdevbase_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_NOTICE "release\r\n");

    return 0;
}

static ssize_t chrdevbase_read(struct file *f, char __user *buf, size_t len, loff_t *off)
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

static ssize_t chrdevbase_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
    char buffer[512];
    int ret;

    printk(KERN_NOTICE "write\r\n");
    if (len > 512) {
        len = 512;
    }
    ret = copy_from_user(buffer, buf, len);
    if (ret < 0) {
        printk(KERN_ERR "copy_from_user failed!\r\n");
        return EFAULT;
    }
    printk(KERN_NOTICE "read %d bytes from userspace: [%s]\r\n", (int)len, buffer);

    return len;
}

struct file_operations chrdevbase_fops = {
    .open = chrdevbase_open,
    .release = chrdevbase_release,
    .read = chrdevbase_read,
    .write = chrdevbase_write
};

static int chrdevbase_init(void)
{
    printk(KERN_NOTICE "init");

    if (register_chrdev(CHRDEVBASE_MAJOR, CHRDEVBASE_NAME, &chrdevbase_fops) < 0) {
        printk(KERN_ERR "register_chrdev failed\r\n");
        return EFAULT;
    }

    return 0;
}

static void chrdevbase_exit(void)
{
    printk(KERN_NOTICE "exit\r\n");

    unregister_chrdev(CHRDEVBASE_MAJOR, CHRDEVBASE_NAME);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dengxh");

module_init(chrdevbase_init);
module_exit(chrdevbase_exit);