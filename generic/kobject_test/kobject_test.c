#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/fs.h>
#include <linux/uacce.h>
#include <linux/device.h>
#include <linux/sysfs.h>

static int __init kobject_test_init(void);
static void __exit kobject_test_exit(void);

// test1的show函数实现
ssize_t test1_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%s", "123");
}

// kobj_attribute for test1
struct kobj_attribute test1_attribute =
    __ATTR(test1, 0644, test1_show, NULL);

// attribute指针数组
struct attribute *attributes[] = {
    &test1_attribute.attr, NULL
};

// attribute_group
struct attribute_group test_attribute_group = {
    .attrs = attributes
};

// kobject对象指针
struct kobject *test_kobject;

static int __init kobject_test_init(void)
{
    int ret = 0;

    // 创建一个kobject对象，并且在sysfs根目录下生成一个kobject_test目录
    test_kobject = kobject_create_and_add("kobject_test", NULL);
    if (test_kobject == NULL) {
        return -ENOMEM;
    }
    printk(KERN_NOTICE "create kobject_test success");

    // kobject_test目录下按attribute_group创建目录项
    ret = sysfs_create_group(test_kobject, &test_attribute_group);
    if (ret < 0) {
        printk(KERN_ERR "call sysfs_create_group failed!");
        return -EFAULT;
    }

    return 0;
}

static void __exit kobject_test_exit(void)
{
    // 删除kobject对象，并且在sys下删除kobject_test这个目录
    // kobject_del(test_kobject);

    kobject_put(test_kobject);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dengxh");

module_init(kobject_test_init);
module_exit(kobject_test_exit);

