#include <linux/init.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/printk.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/miscdevice.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/kfifo.h>

int kfifo1_open(struct inode *ip, struct file *fp)
{
    printk(KERN_NOTICE "open\r\n");
    return 0;
}

int kfifo1_release(struct inode *ip, struct file *fp)
{
    printk(KERN_NOTICE "release\r\n");
    return 0;
}

long kfifo1_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
    printk(KERN_NOTICE "ioctl cmd: %u, arg: %lu", cmd, arg);

    return 0;
}

struct file_operations kfifo1_fops = {
    .open = kfifo1_open,
    .release = kfifo1_release,
    .compat_ioctl = kfifo1_ioctl,
    .unlocked_ioctl = kfifo1_ioctl
};

struct miscdevice misc_dev = {
    .fops = &kfifo1_fops,
    .minor = MISC_DYNAMIC_MINOR,
    .name = "kfifo1"
};

struct kfifo kf;

int __init kfifo1_init(void)
{
    int ret;
    int i;
    // int value;
    printk(KERN_NOTICE "init\r\n");

    ret = kfifo_alloc(&kf, 1024, GFP_KERNEL);
    if (ret < 0) {
        printk(KERN_ERR "kfifo_alloc failed\r\n");
        return -E2BIG;
    }
    misc_register(&misc_dev);

    for (i = 0; i < 128; i++) {
        kfifo_put(&kf, i);
    }
    printk(KERN_DEBUG "kfifo_is_empty: %d\r\n", kfifo_is_empty(&kf));
    printk(KERN_DEBUG "kfifo_len: %d\r\n", kfifo_len(&kf));
    printk(KERN_DEBUG "kfifo_size: %d\r\n", kfifo_size(&kf));
    printk(KERN_DEBUG "kfifo_esize: %d\r\n", kfifo_esize(&kf));

    return 0;
}

void __exit kfifo1_exit(void)
{
    int val;
    int ret;

    while (!kfifo_is_empty(&kf)) {
        ret = kfifo_get(&kf, &val);
        printk(KERN_DEBUG "get val: %d\r\n", val);
        printk(KERN_DEBUG "kfifo_len: %d\r\n", kfifo_len(&kf));
    }

    kfifo_free(&kf);

    misc_deregister(&misc_dev);
    printk(KERN_NOTICE "exit\r\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dengxh");

module_init(kfifo1_init);
module_exit(kfifo1_exit);