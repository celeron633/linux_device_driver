#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dengxh");

#define CCM_CCGR1_PA                0x020C406C
#define IOMUXC_SW_MUX_CTL_PAD_PA    0x020E006C
#define IOMUXC_SW_PAD_CTL_PAD_PA    0x020E02F8
#define GPIOX_GDIR_PA               0x0209C004
#define GPIOX_DR_PA                 0x0209C000

struct resource led_resources[] = {
    [0] = {
        .start = CCM_CCGR1_PA,
        .end = CCM_CCGR1_PA + (4 - 1),
        .flags = IORESOURCE_MEM
    },
    [1] = {
        .start = IOMUXC_SW_MUX_CTL_PAD_PA,
        .end = IOMUXC_SW_MUX_CTL_PAD_PA + (4 - 1),
        .flags = IORESOURCE_MEM
    },
    [2] = {
        .start = IOMUXC_SW_PAD_CTL_PAD_PA,
        .end = IOMUXC_SW_PAD_CTL_PAD_PA + (4 - 1),
        .flags = IORESOURCE_MEM
    },
    [3] = {
        .start = GPIOX_GDIR_PA,
        .end = GPIOX_GDIR_PA + (4 - 1),
        .flags = IORESOURCE_MEM
    },
    [4] = {
        .start = GPIOX_DR_PA,
        .end = GPIOX_DR_PA + (4 - 1),
        .flags = IORESOURCE_MEM
    },
};

void platform_device_led_release(struct device *dev);

struct platform_device led_device = {
    .name = "ebf-imx6ull-led",
    .id = -1,
    .dev.release = platform_device_led_release,
    .resource = led_resources,
    .num_resources = ARRAY_SIZE(led_resources)
};

void platform_device_led_release(struct device *dev)
{
    printk(KERN_NOTICE "platform_device_led_release\r\n");


}

int platform_device_led_init(void)
{
    printk(KERN_NOTICE "platform_device_led_init\r\n");
    platform_device_register(&led_device);
    return 0;
}

void platform_device_led_exit(void)
{
    platform_device_unregister(&led_device);

    printk(KERN_NOTICE "platform_device_led_exit\r\n");
}

module_init(platform_device_led_init);
module_exit(platform_device_led_exit);