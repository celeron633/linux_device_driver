#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dengxh");

struct resource led_resources[] = {
    [0] = {
        .flags = IORESOURCE_MEM
    },
    [1] = {
        .flags = IORESOURCE_MEM
    }
};

struct platform_device led_device = {
    .name = "ebf-imx6ull-led",
    .resource = led_resources,
    .num_resources = ARRAY_SIZE(led_resources)
};

int platform_device_led_init(void)
{
    printk(KERN_NOTICE "platform_device_led_int\r\n");
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