#include "chrdevbase.h"

static int chrdevbase_init(void);
static void chrdevbase_exit(void);
static int chrdevbase_open(struct inode *, struct file *);
static int chrdevbase_release(struct inode *, struct file *);
static ssize_t chrdevbase_read(struct file *f, char __user *buf, size_t len, loff_t *off);
static ssize_t chrdevbase_write(struct file *f, const char __user *buf, size_t len, loff_t *off);
static long chrdevbase_ioctl(struct file *filep, unsigned int cmd, unsigned long para);

struct file_operations chrdevbase_fops = {
    .open = chrdevbase_open,
    .release = chrdevbase_release,
    .read = chrdevbase_read,
    .write = chrdevbase_write,
    .compat_ioctl = chrdevbase_ioctl,
    .unlocked_ioctl = chrdevbase_ioctl
};

// instance
static chrdevbase_t chrdevb;

static int chrdevbase_init(void)
{
    printk(KERN_NOTICE "init");

    // if already with major
    // MKDEV(chrdevb.dev_major, chrdevb.dev_minor);

    // assign dev_t
    alloc_chrdev_region(&(chrdevb.dev_id), 0, 1, CHRDEVBASE_NAME);

    // cdev
    cdev_init(&(chrdevb.cdev), &chrdevbase_fops);
    cdev_add(&(chrdevb.cdev), chrdevb.dev_id, 1);
    
    // class & device
    chrdevb.dev_class = class_create(CHRDEVBASE_NAME);
    chrdevb.dev_device = device_create(chrdevb.dev_class, NULL, chrdevb.dev_id, NULL, CHRDEVBASE_NAME);

    return 0;
}

static void chrdevbase_exit(void)
{
    printk(KERN_NOTICE "exit");

    // cdev
    cdev_del(&(chrdevb.cdev));

    // device & class
    device_destroy(chrdevb.dev_class, chrdevb.dev_id);
    class_destroy(chrdevb.dev_class);

    // chrdevb.dev_class->devnode

    // udev
    unregister_chrdev_region(chrdevb.dev_id, 1);
}

static int chrdevbase_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_NOTICE "open");

    return 0;
}

static int chrdevbase_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_NOTICE "release");

    return 0;
}

static ssize_t chrdevbase_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    char msg_to_user[] = "hello wrorld\n";
    int ret;

    printk(KERN_NOTICE "read");

    ret = copy_to_user(buf, msg_to_user, sizeof(msg_to_user));
    if (ret < 0) {
        printk(KERN_ERR "copy_to_user failed!");
        return EFAULT;
    }
    
    return sizeof(msg_to_user);
}

static ssize_t chrdevbase_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
    char buffer[1024];
    int ret;

    printk(KERN_NOTICE "write");

    ret = copy_from_user(buffer, buf, len);
    if (ret < 0) {
        printk(KERN_ERR "copy_from_user failed!");
        return EFAULT;
    }
    printk(KERN_NOTICE "read %d bytes from userspace: [%s]", (int)len, buffer);

    return len;
}

static long chrdevbase_ioctl(struct file *filep, unsigned int cmd, unsigned long para)
{
    printk(KERN_NOTICE "ioctl");

    switch (cmd)
    {
    case TEST_CMD_1:
        printk(KERN_NOTICE "test cmd 1, para: %d", (int)para);
        break;
    case TEST_CMD_2:
        printk(KERN_NOTICE "test cmd 2, para: %d", (int)para);
        break;
    case TEST_CMD_3:
        printk(KERN_NOTICE "test cmd 3, para: %d", (int)para);
        break;
    case TEST_CMD_4:
        printk(KERN_NOTICE "test cmd 4, para: %d", (int)para);
        break;
    case TEST_CMD_5:
        printk(KERN_NOTICE "test cmd 5, para: %d", (int)para);
        break;    
    default:
        printk(KERN_NOTICE "invalid cmd, para: %d", (int)para);
        return EINVAL;
        break;
    }

    return 0;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dengxh");

module_init(chrdevbase_init);
module_exit(chrdevbase_exit);