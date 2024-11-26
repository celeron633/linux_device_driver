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

typedef struct student {
    struct list_head node;
    char name[24];
    int id;
} student_t;
student_t stu;

int list1_open(struct inode *ip, struct file *fp)
{
    printk(KERN_NOTICE "open\r\n");
    return 0;
}

int list1_release(struct inode *ip, struct file *fp)
{
    printk(KERN_NOTICE "release\r\n");
    return 0;
}

long list1_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
    printk(KERN_NOTICE "ioctl cmd: %u, arg: %lu", cmd, arg);

    return 0;
}

struct file_operations list1_fops = {
    .open = list1_open,
    .release = list1_release,
    .compat_ioctl = list1_ioctl,
    .unlocked_ioctl = list1_ioctl
};

struct miscdevice misc_dev = {
    .fops = &list1_fops,
    .minor = MISC_DYNAMIC_MINOR,
    .name = "list1"
};

void init_stu_list(void)
{
    int i;
    char buf[12];
    struct student *cur;

    INIT_LIST_HEAD(&stu.node);
    for (i = 0; i < 10; i++) {
        struct student *s = kmalloc(sizeof(struct student), GFP_KERNEL);
        s->id = i;
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "%d", i);
        strcpy(s->name, buf);
        list_add(&s->node, &stu.node);
    }

    list_for_each_entry(cur, &stu.node, node) {
        printk("name: %s, id: %d\r\n", cur->name, cur->id);
    }
}

int __init list1_init(void)
{
    printk(KERN_NOTICE "init\r\n");

    init_stu_list();
    misc_register(&misc_dev);

    return 0;
}

void __exit list1_exit(void)
{
    struct student *cur;
    struct student *cur2;

    // 下面注释的代码会引起oops, 以为节点被删除, next时访问了空指针
    // list_for_each_entry(cur, &stu.node, node) {
    //     printk("%d\r\n", cur->id);
    //     list_del(&cur->node);
    //     // BUG
    //     kfree(cur);
    // }

    // 在遍历中有删除或者修改操作, 务必使用带safe的
    list_for_each_entry_safe(cur, cur2, &stu.node, node) {
        printk("del: %d\r\n", cur->id);
        list_del(&cur->node);
        kfree(cur);
    }

    misc_deregister(&misc_dev);
    printk(KERN_NOTICE "exit\r\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dengxh");

module_init(list1_init);
module_exit(list1_exit);