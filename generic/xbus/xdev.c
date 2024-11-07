#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/uacce.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dengxh");

static int __init xdev_init(void)
{
    return 0;

}

static void __exit xdev_exit(void)
{

}

module_init(xdev_init);
module_exit(xdev_exit);