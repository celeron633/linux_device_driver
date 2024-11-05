#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/fs.h>
#include <linux/uacce.h>
#include <linux/device.h>
#include <linux/sysfs.h>

static int __init kobject_test_init(void);
static void __exit kobject_test_exit(void);

struct kobject *test_kobject;

static int __init kobject_test_init(void)
{
    // 创建一个kobject对象，并且在sysfs根目录下生成一个kobject_test目录
    test_kobject = kobject_create_and_add("kobject_test", NULL);
    if (test_kobject == NULL) {
        return -ENOMEM;
    }
    printk(KERN_NOTICE "create kobject_test success");
    // sysfs_create_group()

    return 0;
}

static void __exit kobject_test_exit(void)
{
    // 删除kobject对象，并且在sys下删除kobject_test这个目录
    kobject_del(test_kobject);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dengxh");

module_init(kobject_test_init);
module_exit(kobject_test_exit);

