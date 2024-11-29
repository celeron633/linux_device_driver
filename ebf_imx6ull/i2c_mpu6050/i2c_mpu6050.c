#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/i2c.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>

#include "mpu6050_reg.h"

// i2c_client 
// TODO: 需要mutex保护
struct i2c_client *mpu6050_client;

// mpu6050 i2c 写寄存器函数
int mpu6050_write_reg(struct i2c_client *c, u8 addr, void *data, u32 len)
{
    struct i2c_msg msg_to_send[1];
    int ret;
    char buf[256];

    // buf开头为mpu6050的寄存器地址, 余下来为数据
    buf[0] = addr;
    memcpy(&buf[1], data, len);
    
    msg_to_send[0].addr = c->addr;
    msg_to_send[0].buf = buf;
    msg_to_send[0].flags = 0;
    msg_to_send[0].len = 1 + len;

    ret = i2c_transfer(c->adapter, msg_to_send, 1);
    if (ret < 0) {
        printk(KERN_WARNING "i2c_transfer failed\r\n");
    }
    return ret;
}

int mpu6050_write_one_reg(struct i2c_client *c, u8 addr, u8 data)
{
    char buf;
    int ret;

    ret = mpu6050_write_reg(c, addr, &buf, 1);

    return ret;
}

// mpu6050 i2c 读取函数
int mpu6050_read_reg(struct i2c_client *c, u8 addr, void *data, u32 len)
{
    struct i2c_msg msg_to_send[2];
    int ret;
    
    // 先写后读
    // 第一个消息发送指令 要读的mpu6050寄存器的地址, 不是i2c地址
    msg_to_send[0].addr = c->addr;
    msg_to_send[0].buf = &addr;
    msg_to_send[0].flags = 0;
    msg_to_send[0].len = 1;

    // 第二个消息读取数据, 读取mpu6050返回的寄存器的数据
    msg_to_send[1].addr = c->addr;
    msg_to_send[1].buf = (u8 *)data;
    msg_to_send[1].flags = I2C_M_RD;
    msg_to_send[1].len = len;

    ret = i2c_transfer(c->adapter, msg_to_send, 2);
    if (ret < 0) {
        printk(KERN_WARNING "i2c_transfer failed\r\n");
    }
    return ret;
}

u8 mpu6050_read_one_reg(struct i2c_client *c, u8 addr)
{
    u8 buf;
    u8 ret;

    ret = mpu6050_read_reg(c, addr, &buf, 1);

    return buf;
}

int mpu6050_internal_init(struct i2c_client *c)
{
    int ret = 0;
    u8 mpu6050_id;
    
    ret += mpu6050_write_one_reg(c, MPU6050_RA_PWR_MGMT_1, 0x80);
    mdelay(50);
    ret += mpu6050_write_one_reg(c, MPU6050_RA_PWR_MGMT_1, 0x01);
    mdelay(50);

    mpu6050_id = mpu6050_read_one_reg(c, MPU6050_RA_WHO_AM_I);
    printk(KERN_NOTICE "mpu6050 ID = %02x\r\n", mpu6050_id);

    if (ret < 0) {
        printk(KERN_WARNING "mpu6050_internal_init failed\r\n");
        return -1;
    } else {
        printk(KERN_DEBUG "mpu6050_internal_init success\r\n");
    }

    return 0;
}

// file operations
static int mpu6050_open(struct inode *inodep, struct file *filep)
{
    int ret;
    printk(KERN_NOTICE "open\r\n");

    ret = mpu6050_internal_init(mpu6050_client);
    if (ret < 0) {
        return -EXDEV;
    }

    return 0;
}

static int mpu6050_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_NOTICE "release\r\n");

    return 0;
}

static ssize_t mpu6050_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_NOTICE "mpu6050 read\r\n");

    
    
    return 0;
}

static ssize_t mpu6050_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
    // int ret;

    printk(KERN_NOTICE "mpu6050 write\r\n");

    return len;
}

struct file_operations mpu6050_fops = {
    .open = mpu6050_open,
    .release = mpu6050_release,
    .read = mpu6050_read,
    .write = mpu6050_write,
};

// misc device
struct miscdevice mpu6050_misc = {
    .fops = &mpu6050_fops,
    .minor = MISC_DYNAMIC_MINOR,
    .name = "fire-mpu6050"
};

static int i2c_mpu6050_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    printk(KERN_NOTICE "i2c_mpu6050_probe, %s matched\r\n", id->name);

    mpu6050_client = client;
    misc_register(&mpu6050_misc);

    return 0;
}

static int i2c_mpu6050_remove(struct i2c_client *cleint)
{
    printk(KERN_NOTICE "i2c_mpu6050_remove\r\n");
    misc_deregister(&mpu6050_misc);

    return 0;
}

// register to i2c bus, not platform
static struct of_device_id mpu6050_of_match[] = {
    // invensense,mpu6050 defined in overlay dts
    {.compatible = "invensense,mpu6050"},
    {}
};

static struct i2c_device_id mpu6050_id[] = {
    {.name = "mpu6050", .driver_data = 0},
    {}
};

// 若i2c devices已经被注册, match则无效, 不会继续匹配
static struct i2c_driver mpu6050_driver = {
    .probe = i2c_mpu6050_probe,
    .remove = i2c_mpu6050_remove,
    .driver = {
        .owner = THIS_MODULE,
        .of_match_table = mpu6050_of_match,
        .name = "fire-mpu6050"
    },
    .id_table = mpu6050_id
};

static int __init i2c_mpu6050_init(void)
{
    int ret;
    printk(KERN_NOTICE "i2c_mpu6050_init\r\n");

    ret = i2c_add_driver(&mpu6050_driver);
    if (ret < 0) {
        printk(KERN_ERR "i2c_add_driver failed\r\n");
        return -EINVAL;
    }

    return 0;
}

static void __exit i2c_mpu6050_exit(void)
{
    printk(KERN_NOTICE "i2c_mpu6050_exit\r\n");

    printk(KERN_DEBUG "i2c_del_driver begin\r\n");
    i2c_del_driver(&mpu6050_driver);
    printk(KERN_DEBUG "i2c_del_driver end\r\n");

}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dengxh");

module_init(i2c_mpu6050_init);
module_exit(i2c_mpu6050_exit);