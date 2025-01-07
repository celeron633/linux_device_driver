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

#include "reg_max7219.h"

static struct spi_device *max7219_device;

static struct spi_device_id max7219_id[] = {
    {.name = "max7219"}
};

static struct of_device_id max7219_of_id[] = {
    {.compatible = "max7219", .name = "max7219"}
};

static int max7219_write_reg(struct spi_device *device, uint8_t opcode, uint8_t data)
{
    uint8_t buf[2];
    buf[0] = opcode;
    buf[1] = data;

    if (spi_write(device, buf, 2) < 0) {
        pr_err("spi_write FAILED!\r\n");
        return -1;
    }

    return 0;
}

static int max7219_clear(struct spi_device *device, uint8_t count)
{
    int i;
    for(i = 0; i < count; i++) {
        if (max7219_write_reg(device, i+1, 0x00) < 0) {
            return -1;
        }
    }

    return 0;
}

static int max7219_set_brightness(struct spi_device *device, uint8_t brightness)
{
    // 0x0f: 最大16个级别
    brightness &= 0x0f;
    if (max7219_write_reg(device, REG_INTENSITY, brightness) < 0) {
        return -1;
    }

    return 0;
}

static int max7219_init(struct spi_device *device)
{
    pr_debug("max7219 init begin\r\n");

    // 初始化7219寄存器
    if (max7219_write_reg(device, REG_SCAN_LIMIT, 0x07) < 0)
        return -1;
    if (max7219_write_reg(device, REG_DECODE, 0x00) < 0)
        return -1;
    if (max7219_write_reg(device, REG_SHUTDOWN, 0x01) < 0)
        return -1;
    if (max7219_write_reg(device, REG_DISPLAY_TEST, 0x00) < 0)
        return -1;
    if (max7219_clear(device, 0x08))
        return -1;
    if (max7219_set_brightness(device, INTENSITY_MAX) < 0)
        return -1;

    pr_debug("max7219 init end\r\n");
    return 0;
}

static int max7219_probe(struct spi_device *spi)
{
    pr_debug("max7219_probe begin!\r\n");
    // 保存device指针
    max7219_device = spi;

    // show MODE
    pr_debug("spi device mode: [%u]\r\n", spi->mode);

    // 初始化max7219
    max7219_init(spi);

    pr_debug("max7219_probe end!\r\n");

    return 0;
}

static struct spi_driver max7219_driver = {
    .driver = {
        .name = "spi_max7219",
        .owner = THIS_MODULE,
        .of_match_table = max7219_of_id
    },
    .id_table = max7219_id,
    .probe = &max7219_probe
};

static int max7219_misc_fop_open(struct inode *i, struct file *f)
{
    pr_debug("max7219 open\r\n");
    f->private_data = max7219_device;

    return 0;
}

static int max7219_misc_fop_release(struct inode *i, struct file *f)
{
    pr_debug("max7219 release\r\n");

    return 0;
}

static ssize_t max7219_misc_fop_write(struct file *f, const char __user *buf, size_t size, loff_t *off)
{
    char k_buffer[16];
    int ret;

    pr_debug("max7219 write\r\n");
    if (size > 16)
        size = 16;
    ret = copy_from_user(k_buffer, buf, size);
    if (ret < 0) {
        pr_err("copy_from_user FAILED!\r\n");
        return -EBADR;
    }
    pr_debug("msg: [%s]\r\n", k_buffer);

    // 显示到数码管


    return size;
}

struct file_operations max7219_misc_fops = {
    .open = &max7219_misc_fop_open,
    .release = &max7219_misc_fop_release,
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