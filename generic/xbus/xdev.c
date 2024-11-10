#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/uacce.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dengxh");

void xdev_release(struct device *dev)
{

}

extern struct bus_type xbus;

struct device xdev = {
    .init_name = "xdev",
    .release = xdev_release,
    .bus = &xbus
};
EXPORT_SYMBOL(xdev);

static int __init xdev_init(void)
{
    int ret;

    ret = device_register(&xdev);
    if (ret < 0) {
        printk(KERN_ERR "device_register for xdev failed\r\n");
        return -EFAULT;
    } else {
        printk(KERN_NOTICE "device_register for xdev success\r\n");
    }

    return 0;
}

static void __exit xdev_exit(void)
{
    device_unregister(&xdev);
}

module_init(xdev_init);
module_exit(xdev_exit);