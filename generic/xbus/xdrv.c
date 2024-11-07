#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/uacce.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dengxh");

static int __init xdrv_init(void)
{
    return 0;
}

static void __exit xdrv_exit(void)
{

}

module_init(xdrv_init);
module_exit(xdrv_exit);