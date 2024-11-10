#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/uacce.h>
#include <linux/fs.h>
#include <linux/printk.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dengxh");

static int xbus_match(struct device *dev, struct device_driver *drv)
{
    if (strcmp(dev_name(dev), drv->name) == 0) {
        printk(KERN_NOTICE "[drv: %s, dev: %s] matched\r\n", drv->name, dev_name(dev));
        return 1;
    } else {
        printk(KERN_NOTICE "[drv: %s, dev: %s] not match\r\n", drv->name, dev_name(dev));
    }

    return 0;
}

struct bus_type xbus = {
    .name = "xbus",
    .match = xbus_match
};
EXPORT_SYMBOL(xbus);

static int __init xbus_init(void)
{
    int ret;

    ret = bus_register(&xbus);
    if (ret < 0) {
        printk(KERN_WARNING "bus_register failed!\r\n");
        return -EFAULT;
    }
    printk(KERN_NOTICE "bus_register success\r\n");

    return 0;
}

static void __exit xbus_exit(void)
{
    bus_unregister(&xbus);
}

module_init(xbus_init);
module_exit(xbus_exit);