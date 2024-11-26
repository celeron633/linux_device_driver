#include <linux/init.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/printk.h>
#include <linux/miscdevice.h>

struct student {
    struct list_head node;
    char name[24];
    int id;
};

struct student stu;

void init_stu_list(void)
{
    int i;
    char buf[12];
    INIT_LIST_HEAD(&stu.node);

    for (i = 0; i < 10; i++) {
        struct student *s = kmalloc(sizeof(struct student), GFP_KERNEL);
        s->id = i;
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "%d", i);
        strcpy(s->name, buf);
        list_add(&s->node, &stu.node);
    }

    struct student *cur;
    list_for_each_entry(cur, &stu.node, node) {
        printk("name: %s, id: %d\r\n", cur->name, cur->id);
    }
}

int __init list1_init(void)
{
    printk(KERN_NOTICE "init\r\n");

    init_stu_list();

    return 0;
}

void __exit list1_exit(void)
{
    printk(KERN_NOTICE "exit\r\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dengxh");

module_init(list1_init);
module_exit(list1_exit);