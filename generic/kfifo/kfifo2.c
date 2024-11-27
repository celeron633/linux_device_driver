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

int kfifo2_open(struct inode *ip, struct file *fp)
{
    printk(KERN_NOTICE "open\r\n");
    return 0;
}

int kfifo2_release(struct inode *ip, struct file *fp)
{
    printk(KERN_NOTICE "release\r\n");
    return 0;
}

long kfifo2_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
    printk(KERN_NOTICE "ioctl cmd: %u, arg: %lu", cmd, arg);

    return 0;
}

struct file_operations kfifo2_fops = {
    .open = kfifo2_open,
    .release = kfifo2_release,
    .compat_ioctl = kfifo2_ioctl,
    .unlocked_ioctl = kfifo2_ioctl
};

struct miscdevice misc_dev = {
    .fops = &kfifo2_fops,
    .minor = MISC_DYNAMIC_MINOR,
    .name = "kfifo2"
};

struct kfifo kf;
u8 kf_buffer[2048];

int __init kfifo2_init(void)
{
    int ret;
    int i;
    char buf[1];

    printk(KERN_NOTICE "init\r\n");
    misc_register(&misc_dev);

    ret = kfifo_init(&kf, kf_buffer, 2048);

    for (i = 0; i < 1024; i++) {
        buf[0] = (i % 255);
        kfifo_in(&kf, buf, 1);
    }
    printk(KERN_DEBUG "kfifo_is_empty: %d\r\n", kfifo_is_empty(&kf));
    printk(KERN_DEBUG "kfifo_len: %d\r\n", kfifo_len(&kf));
    printk(KERN_DEBUG "kfifo_size: %d\r\n", kfifo_size(&kf));
    printk(KERN_DEBUG "kfifo_esize: %d\r\n", kfifo_esize(&kf));

    return 0;
}

void __exit kfifo2_exit(void)
{
    int ret;
    char buf[1];

    while (!kfifo_is_empty(&kf)) {
        ret = kfifo_out(&kf, buf, 1);
        printk(KERN_DEBUG "val: 0x%02X, len: %d\r\n", buf[0], kfifo_len(&kf));
    }

    // free to a pre-allocated buffer with opps
    // kfifo_free(&kf);

    misc_deregister(&misc_dev);
    printk(KERN_NOTICE "exit\r\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dengxh");

module_init(kfifo2_init);
module_exit(kfifo2_exit);