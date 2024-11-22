#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/file.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/timer.h>

struct timer_list timer;

void timer_callback(struct timer_list *t)
{
    printk(KERN_NOTICE "timer_callback, jiffies: %lu\r\n", jiffies);
    mod_timer(&timer, jiffies + msecs_to_jiffies(1000));
}

static int __init timer_test_init(void)
{
    printk(KERN_NOTICE "timer_test_init\r\n");
    
    timer_setup(&timer, timer_callback, 0);
    timer.expires = jiffies + msecs_to_jiffies(1000);
    add_timer(&timer);

    return 0;
}

static void __exit timer_test_exit(void)
{
    printk(KERN_NOTICE "timer_test_exit\r\n");

    timer_delete(&timer);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dengxh");

module_init(timer_test_init);
module_exit(timer_test_exit);