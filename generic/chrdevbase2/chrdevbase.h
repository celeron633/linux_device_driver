#ifndef __CHRDEVBASE_H__
#define __CHRDEVBASE_H__

#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/file.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/device/class.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/module.h>

#define CHRDEVBASE_NAME "chrdevbase"

enum CHRDEVBASE_IOCTL_CMD {
    TEST_CMD_1 = 1,
    TEST_CMD_2,
    TEST_CMD_3,
    TEST_CMD_4,
    TEST_CMD_5
};

typedef struct chrdevbase_t
{
    int dev_major;
    int dev_minor;
    dev_t dev_id;
    struct cdev cdev;
    struct device *dev_device;
    struct class *dev_class;
} chrdevbase_t;


#endif