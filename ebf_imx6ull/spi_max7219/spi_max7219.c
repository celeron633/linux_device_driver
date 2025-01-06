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
#include <linux/spi/spi.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>

static struct spi_device *max7219_device;

static struct spi_device_id max7219_id[] = {
    {.name = "max7219"},
    0
};

static int max7219_init(struct spi_device *device)
{
    pr_debug("max7219 init\r\n");

    // 写7219寄存器

}

static int max7219_probe(struct spi_device *spi)
{
    pr_debug("max7219_probe begin!\r\n");
    max7219_device = spi;
    pr_debug("max7219_probe end!\r\n");

    // 初始化max7219
    max7219_init(spi);
}

static struct spi_driver max7219_driver = {
    .id_table = &max7219_id,
    .probe = &max7219_probe
};

static int max7219_misc_fop_open(struct inode *i, struct file *f)
{
    pr_debug("max7219 open\r\n");
    f->private_data = max7219_device;
}

static int max7219_misc_fop_close(struct inode *i, struct file *f)
{
    pr_debug("max7219 close\r\n");

}

static ssize_t max7219_misc_fop_write(struct file *f, const char __user *buf, size_t size, loff_t *off)
{
    pr_debug("max7219 write\r\n");

}

struct file_operations max7219_misc_fops = {
    .open = &max7219_misc_fop_open,
    .release = &max7219_misc_fop_close,
    .write = &max7219_misc_fop_write
};

struct miscdevice max7219_misc = {
    .name = "spi_max7219",
    .minor = MISC_DYNAMIC_MINOR,
    .fops = &max7219_misc_fops
};

static int __init spi_max7219_init(void)
{
    int ret;
    pr_debug("spi_max7219_init\r\n");
    // 注册spi driver
    ret = spi_register_driver(&max7219_driver);
    if (ret < 0) {
        pr_err("spi_register_driver failed\r\n");
        return -EINVAL;
    }

    // 注册misc设备
    ret = misc_register(&max7219_misc);
    if (ret < 0) {
        pr_err("misc_register failed!\r\n");
        return -EINVAL;
    }

    return 0;
}

static void __exit spi_max7219_exit(void)
{
    pr_debug("spi_max7219_exit\r\n");

    pr_debug("i2c_del_driver begin\r\n");
    pr_debug("i2c_del_driver end\r\n");

}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dengxh");

module_init(spi_max7219_init);
module_exit(spi_max7219_exit);