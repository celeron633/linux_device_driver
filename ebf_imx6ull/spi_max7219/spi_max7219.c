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

int max7219_probe(struct spi_device *spi)
{
    pr_debug("max7219_probe begin!\r\n");
    max7219_device = spi;
    pr_debug("max7219_probe end!\r\n");
}

static struct spi_driver max7219_driver = {
    .id_table = &max7219_id,
    .probe = &max7219_probe
};

struct file_operations max7219_misc_fops = {

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