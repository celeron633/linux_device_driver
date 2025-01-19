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

// 写max7219寄存器
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

// 清屏
static int max7219_clear(struct spi_device *device, uint8_t count)
{
    int i;
    for(i = 0; i < count; i++) {
        if (max7219_write_reg(device, i+1, 0x00) < 0) {
            pr_err("spi_write FAILED!\r\n");
            return -1;
        }
    }

    return 0;
}

// 设置亮度
static int max7219_set_brightness(struct spi_device *device, uint8_t brightness)
{
    // 0x0f: 最大16个级别
    brightness &= 0x0f;
    if (max7219_write_reg(device, REG_INTENSITY, brightness) < 0) {
        return -1;
    }

    return 0;
}

// max7219初始化
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

// platform driver的probe函数
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

// 字符转换为数码管的编码
static unsigned char max7219_lookup_code(char ch, unsigned int dp)
{
    int dot = 0;
    int i;

    if (dp)
        dot = 1;
    // 大小写
    if (ch >= 35 && ch <= 44) {
        ch += 13;
        dot = 1;
    }
    for (i = 0; MAX7219_Font[i].ascii; i++) {
        if (ch == MAX7219_Font[i].ascii) {
            char tmp_seg = MAX7219_Font[i].segs;
            if (dot)
                tmp_seg |= (1<<7);

            return tmp_seg;
        }
    }
    return 0;
}

// max7219显示字符
static int max7219_display_char(int index, char value, uint8_t dp)
{
    if (max7219_write_reg(max7219_device, index, max7219_lookup_code(value, dp)) < 0) {
        pr_err("spi_write FAILED!\r\n");
        return -1;
    }

    return 0;
}

// 转换字符串到max7219编码数组并输出
static int max7219_display_text(const char *text, int mode)
{
    char trimStr[16] = {0};
    int  decimal[16] = {0};
    int err_flag = 0;
    int i = 0, j = 0;


    int len = strlen(text);
    if (len > 16)
        len = 16;
    
    for (; i < len; i++) {
        if (text[i] == '.') {
            if (j > 1) {
                decimal[j-1] = 1;
            } else {
                decimal[0] = 1;
            }
        } else {
            trimStr[j] = text[i];
            j++;
        }
    }

    if (j > 8)
        j = 8;
    switch (mode) {
        case 0:
            for (i = 0; i < j; i++) {
                if (max7219_display_char(j-i, trimStr[i], decimal[i]) < 0) {
                    err_flag = 1;
                }
            }
            break;
        case 1:
            for (i = 0; i < j; i++) {
                if (max7219_display_char(8-i, trimStr[i], decimal[i]) < 0) {
                    err_flag = 1;
                }
            }
            break;
        default:
            break;
    }

    if (err_flag == 1) {
        pr_err("spi error while sending data!\r\n");
        return -1;
    }

    return 0;
}

static ssize_t max7219_misc_fop_write(struct file *f, const char __user *buf, size_t size, loff_t *off)
{
    char k_buffer[16];
    int ret;

    memset(k_buffer, 0x00, sizeof(k_buffer));
    pr_debug("max7219 write\r\n");
    if (size > 16)
        size = 16;
    ret = copy_from_user(k_buffer, buf, size);
    if (ret < 0) {
        pr_err("copy_from_user FAILED!\r\n");
        return -EBUSY;
    }
    // 去掉\r\n
    if (size >= 1)
        k_buffer[size-1] = 0;
    pr_debug("msg: [%s], ret: [%d], size: [%d]\r\n", k_buffer, ret, size);

    // 显示到数码管
    max7219_display_text(k_buffer, 0);

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
    .fops = &max7219_misc_fops,
    .mode = O_RDWR
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

    misc_deregister(&max7219_misc);

    pr_debug("spi_unregister_driver begin\r\n");
    spi_unregister_driver(&max7219_driver);
    pr_debug("spi_unregister_driver end\r\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dengxh");

module_init(spi_max7219_init);
module_exit(spi_max7219_exit);