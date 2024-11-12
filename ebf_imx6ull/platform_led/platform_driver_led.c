#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/platform_device.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dengxh");

int platform_driver_led_probe(struct platform_device *pdev);

struct platform_driver led_driver = {
    .probe = platform_driver_led_probe,
    .driver.name = "ebf-imx6ull-led"
};

struct resource *res_ccm_ccgr1;
struct resource *res_iomux_sw_mux_ctl_pad;
struct resource *res_iomux_sw_pad_ctl_pad;
struct resource *res_gpio_gdir;
struct resource *res_gpio_dr;

int platform_driver_led_probe(struct platform_device *pdev)
{
    printk(KERN_NOTICE "probe!\r\n");

    res_ccm_ccgr1 = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    res_iomux_sw_mux_ctl_pad = platform_get_resource(pdev, IORESOURCE_MEM, 1);
    res_iomux_sw_pad_ctl_pad = platform_get_resource(pdev, IORESOURCE_MEM, 2);
    res_gpio_gdir = platform_get_resource(pdev, IORESOURCE_MEM, 3);
    res_gpio_dr = platform_get_resource(pdev, IORESOURCE_MEM, 4);

    return 0;
}

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