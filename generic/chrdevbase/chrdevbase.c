#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/file.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/module.h>


#define CHRDEVBASE_MAJOR 200
#define CHRDEVBASE_NAME "chrdevbase"

int chrdevbase_init(void);
void chrdevbase_exit(void);
int chrdevbase_open(struct inode *, struct file *);
int chrdevbase_release(struct inode *, struct file *);
ssize_t chrdevbase_read(struct file *f, char __user *buf, size_t len, loff_t *off);
ssize_t chrdevbase_write(struct file *f, const char __user *buf, size_t len, loff_t *off);

struct file_operations chrdevbase_fops = {
    .open = chrdevbase_open,
    .release = chrdevbase_release,
    .read = chrdevbase_read,
    .write = chrdevbase_write
};

char r_buffer[2048] = {0};
char w_buffer[2048] = "hello world\n";

int chrdevbase_init(void)
{
    printk(KERN_NOTICE "init");

    if (register_chrdev(CHRDEVBASE_MAJOR, CHRDEVBASE_NAME, &chrdevbase_fops) < 0) {
        printk(KERN_ERR "register_chrdev failed");
        return EFAULT;
    }

    return 0;
}

void chrdevbase_exit(void)
{
    printk(KERN_NOTICE "exit");

    unregister_chrdev(CHRDEVBASE_MAJOR, CHRDEVBASE_NAME);
}

int chrdevbase_open(struct inode *, struct file *)
{
    printk(KERN_NOTICE "open");
    return 0;
}

int chrdevbase_release(struct inode *, struct file *)
{
    printk(KERN_NOTICE "release");
    return 0;
}

ssize_t chrdevbase_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_NOTICE "read");
    unsigned long ret_val = copy_to_user(buf, w_buffer, strlen(w_buffer));
    printk(KERN_NOTICE "write %lu bytes to userspace", ret_val);
    return ret_val;
}

ssize_t chrdevbase_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_NOTICE "write");
    int ret_val = copy_from_user(r_buffer, buf, len);
    printk(KERN_NOTICE "read %d bytes from userspace", ret_val);
    return ret_val;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dengxh");

module_init(chrdevbase_init);
module_exit(chrdevbase_exit);