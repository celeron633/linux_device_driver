#ifndef __CHRDEVBASE_H__
#define __CHRDEVBASE_H__

#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/file.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/module.h>

#include <asm/io.h>

#define LED_DEV_NAME "led1"

// clock
#define CCM_CCGR1_PHY                           0x020C406C

// mux && pad
#define IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO04_PHY    0x20E00006
#define IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO04_PHY    0x20E002F8

// dr & gdir
#define GPIO1_DR_PHY                            0x2090C000
#define GPIO1_GDIR_PHY                          0x2090C004


enum LED_DEV_IOCTL_CMD {
    LED_ON_CMD = 1,
    LED_OFF_CMD = 2,
    LED_TOGGLE_CMD = 3
};

struct led_dev_va_t {
    void *CCM_CCGR1_VA;
    void *IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO04_VA;
    void *IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO04_VA;
    void *GPIO1_DR_VA;
    void *GPIO1_GDIR_VA;
};

typedef struct led_dev_t
{
    int dev_major;
    int dev_minor;
    dev_t dev_id;
    struct cdev dev_cdev;
    struct device *dev_device;
    struct class *dev_class;

    struct led_dev_va_t dev_va;
} led_dev_t;


#endif