#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/device/driver.h>
#include <linux/uacce.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dengxh");

extern struct bus_type xbus;

int xdrv_probe(struct device *dev)
{
    return 0;
}

struct device_driver xdrv = {
    .name = "xdrv",
    .bus = &xbus,
    .probe = xdrv_probe
};
EXPORT_SYMBOL(xdrv);

static int __init xdrv_init(void)
{
    int ret = 0;

    ret = driver_register(&xdrv);
    if (ret < 0) {
        printk(KERN_ERR "driver_register for xdrv failed\r\n");
        return -EFAULT;
    } else {
        printk(KERN_NOTICE "driver_register for xdrv success\r\n");
    }

    return 0;
}

static void __exit xdrv_exit(void)
{
    driver_unregister(&xdrv);
}

module_init(xdrv_init);
module_exit(xdrv_exit);