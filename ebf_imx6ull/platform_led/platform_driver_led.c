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

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dengxh");

int platform_driver_led_probe(struct platform_device *pdev);

static dev_t led_dev;

struct platform_driver led_driver = {
    .probe = platform_driver_led_probe,
    .driver.name = "ebf-imx6ull-led"
};

static struct resource *res_ccm_ccgr1;
static struct resource *res_iomux_sw_mux_ctl_pad;
static struct resource *res_iomux_sw_pad_ctl_pad;
static struct resource *res_gpio_gdir;
static struct resource *res_gpio_dr;

static void __iomem *ccm_ccgr1_va;
static void __iomem *iomux_sw_mux_ctl_pad_va;
static void __iomem *iomux_sw_pad_ctl_pad_va;
static void __iomem *gpio_gdir_va;
static void __iomem *gpio_dr_va;

int platform_driver_led_probe(struct platform_device *pdev)
{
    u32 val = 0;

    printk(KERN_NOTICE "probe!\r\n");

    res_ccm_ccgr1 = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    res_iomux_sw_mux_ctl_pad = platform_get_resource(pdev, IORESOURCE_MEM, 1);
    res_iomux_sw_pad_ctl_pad = platform_get_resource(pdev, IORESOURCE_MEM, 2);
    res_gpio_gdir = platform_get_resource(pdev, IORESOURCE_MEM, 3);
    res_gpio_dr = platform_get_resource(pdev, IORESOURCE_MEM, 4);

    printk(KERN_NOTICE "ccgr1: %x\r\n", res_ccm_ccgr1->start);
    printk(KERN_NOTICE "mux: %x\r\n", res_iomux_sw_mux_ctl_pad->start);
    printk(KERN_NOTICE "pad: %x\r\n", res_iomux_sw_pad_ctl_pad->start);
    printk(KERN_NOTICE "gdir: %x\r\n", res_gpio_gdir->start);
    printk(KERN_NOTICE "dr: %x\r\n", res_gpio_dr->start);

    ccm_ccgr1_va = ioremap(res_ccm_ccgr1->start, resource_size(res_ccm_ccgr1));
    iomux_sw_mux_ctl_pad_va = ioremap(res_iomux_sw_mux_ctl_pad->start, resource_size(res_iomux_sw_mux_ctl_pad));
    iomux_sw_pad_ctl_pad_va = ioremap(res_iomux_sw_pad_ctl_pad->start, resource_size(res_iomux_sw_pad_ctl_pad));
    gpio_gdir_va = ioremap(res_gpio_gdir->start, resource_size(res_gpio_gdir));
    gpio_dr_va = ioremap(res_gpio_dr->start, resource_size(res_gpio_dr));

    // CCGR
    val = readl(ccm_ccgr1_va);
    val &= ~(3 << 26);
    val |= (3 << 26);
    writel(val, ccm_ccgr1_va);

    // MUX
    writel(5, res_iomux_sw_mux_ctl_pad);
    // PAD
    writel(0x10B0, res_iomux_sw_pad_ctl_pad);

    // GDIR (set input or output, set to 1 as output)
    val = readl(gpio_gdir_va);
    val &= ~(1 << 4);
    val |= (1 << 4);
    writel(val, gpio_gdir_va);
    
    // DR (data register)
    val = readl(gpio_dr_va);
    val |= (1 << 4);
    writel(val, gpio_dr_va);

    alloc_chrdev_region(&led_dev, 0, 1, "ebf-led");
    // cdev_init(&led_dev, )

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