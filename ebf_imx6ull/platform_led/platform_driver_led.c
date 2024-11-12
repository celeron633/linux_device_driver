#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/platform_device.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dengxh");

int platform_driver_led_probe(struct platform_device *pdev)
{
    printk(KERN_NOTICE "probe!\r\n");
    return 0;
}

struct platform_driver led_driver = {
    .probe = platform_driver_led_probe
};

int platform_driver_led_init(void)
{
    printk(KERN_NOTICE "platform_driver_led_init\r\n");
    platform_driver_register(&led_driver);

    return 0;
}

void platform_driver_led_exit(void)
{
    printk(KERN_NOTICE "platform_driver_led_exit\r\n");
    platform_driver_unregister(&led_driver);;
}

module_init(platform_driver_led_init);
module_exit(platform_driver_led_exit);