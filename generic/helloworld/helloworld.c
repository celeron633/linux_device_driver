#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/file.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/module.h>


#define helloworld_MAJOR 200
#define helloworld_NAME "helloworld"

int helloworld_init(void);
void helloworld_exit(void);

int helloworld_init(void)
{
    printk(KERN_NOTICE "hello world init\r\n");
    return 0;
}

void helloworld_exit(void)
{
    printk(KERN_NOTICE "hello world exit\r\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dengxh");

module_init(helloworld_init);
module_exit(helloworld_exit);
