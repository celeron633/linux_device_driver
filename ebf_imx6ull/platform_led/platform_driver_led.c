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
int platform_driver_led_remove(struct platform_device *ppdev);

static dev_t led_dev_id;
static struct cdev led_cdev;
static struct class *led_class;
static struct device *led_device;

static int led_dev_open(struct inode *inodep, struct file *filep);
static int led_dev_release(struct inode *inodep, struct file *filep);
static ssize_t led_dev_read(struct file *f, char __user *buf, size_t len, loff_t *off);
static ssize_t led_dev_write(struct file *f, const char __user *buf, size_t len, loff_t *off);
static long led_dev_ioctl(struct file *filep, unsigned int cmd, unsigned long para);

struct platform_driver led_driver = {
    .probe = platform_driver_led_probe,
    .remove = platform_driver_led_remove,
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

enum LED_DEV_IOCTL_CMD {
    LED_ON_CMD = 1,
    LED_OFF_CMD = 2,
    LED_TOGGLE_CMD = 3
};

struct file_operations led_dev_fops = {
    .open = led_dev_open,
    .release = led_dev_release,
    .read = led_dev_read,
    .write = led_dev_write,
    .compat_ioctl = led_dev_ioctl,
    .unlocked_ioctl = led_dev_ioctl
};

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

    ccm_ccgr1_va = ioremap(res_ccm_ccgr1->start, 4);
    iomux_sw_mux_ctl_pad_va = ioremap(res_iomux_sw_mux_ctl_pad->start, 4);
    iomux_sw_pad_ctl_pad_va = ioremap(res_iomux_sw_pad_ctl_pad->start, 4);
    gpio_gdir_va = ioremap(res_gpio_gdir->start, 4);
    gpio_dr_va = ioremap(res_gpio_dr->start, 4);

    printk(KERN_NOTICE "ccgr1 va : %p\r\n", ccm_ccgr1_va);
    printk(KERN_NOTICE "mux va : %p\r\n", iomux_sw_mux_ctl_pad_va);
    printk(KERN_NOTICE "pad va: %p\r\n", iomux_sw_pad_ctl_pad_va);
    printk(KERN_NOTICE "gdir va: %p\r\n", gpio_gdir_va);
    printk(KERN_NOTICE "dr va: %p\r\n", gpio_dr_va);

    // CCGR
    val = readl(ccm_ccgr1_va);
    val &= ~(3 << 26);
    val |= (3 << 26);
    writel(val, ccm_ccgr1_va);

    // MUX
    writel(5, iomux_sw_mux_ctl_pad_va);
    // PAD
    writel(0x10B0, iomux_sw_pad_ctl_pad_va);

    // GDIR (set input or output, set to 1 as output)
    val = readl(gpio_gdir_va);
    val &= ~(1 << 4);
    val |= (1 << 4);
    writel(val, gpio_gdir_va);
    
    // DR (data register)
    val = readl(gpio_dr_va);
    val |= (1 << 4);
    writel(val, gpio_dr_va);

    // dev_t
    alloc_chrdev_region(&led_dev_id, 0, 1, "ebf-led");

    // cdev
    cdev_init(&led_cdev, &led_dev_fops);
    cdev_add(&led_cdev, led_dev_id, 1);

    // class & device
    led_class = class_create(THIS_MODULE, "ebf-led-class");
    led_device = device_create(led_class, NULL, led_dev_id, NULL, "ebf-led");

    return 0;
}

int platform_driver_led_remove(struct platform_device *ppdev)
{
    printk(KERN_NOTICE "iounmap begin\r\n");
    iounmap(ccm_ccgr1_va);
    iounmap(iomux_sw_mux_ctl_pad_va);
    iounmap(res_iomux_sw_pad_ctl_pad);
    iounmap(gpio_gdir_va);
    iounmap(gpio_dr_va);
    printk(KERN_NOTICE "iounmap end\r\n");

    printk(KERN_NOTICE "cdev_del begin\r\n");
    cdev_del(&led_cdev);
    printk(KERN_NOTICE "cdev_del end\r\n");

    printk(KERN_NOTICE "device_destroy begin\r\n");
    device_destroy(led_class, led_dev_id);
    printk(KERN_NOTICE "device_destroy end\r\n");

    printk(KERN_NOTICE "class_destroy begin\r\n");
    class_destroy(led_class);
    printk(KERN_NOTICE "class_destroy end\r\n");

    printk(KERN_NOTICE "unregister_chrdev_region begin\r\n");
    unregister_chrdev_region(led_dev_id, 1);
    printk(KERN_NOTICE "unregister_chrdev_region end\r\n");

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

    printk(KERN_NOTICE "platform_driver_unregister begin\r\n");
    platform_driver_unregister(&led_driver);
    printk(KERN_NOTICE "platform_driver_unregister end\r\n");
}

// old functions
static int led_dev_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_NOTICE "open\r\n");

    return 0;
}

static int led_dev_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_NOTICE "release\r\n");

    return 0;
}

static ssize_t led_dev_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    char msg_to_user[] = "hello wrorld\r\n";
    int ret;

    printk(KERN_NOTICE "read\r\n");

    ret = copy_to_user(buf, msg_to_user, sizeof(msg_to_user));
    if (ret < 0) {
        printk(KERN_ERR "copy_to_user failed!\r\n");
        return EFAULT;
    }
    
    return sizeof(msg_to_user);
}

static ssize_t led_dev_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
    char buffer[256];
    int ret;
    int val;

    printk(KERN_NOTICE "write\r\n");

    ret = copy_from_user(buffer, buf, len);
    if (ret < 0) {
        printk(KERN_ERR "copy_from_user failed!\r\n");
        return EFAULT;
    }
    printk(KERN_NOTICE "read %d bytes from userspace: [%s]\r\n", (int)len, buffer);

    if (strlen(buffer) > 1) {
        ret = buffer[0] - '0';
    }

    // DR (data register)
    val = readl(gpio_dr_va);
    val &= ~(1 << 4);
    if (ret == 0) {
        val |= (0 << 4);
    } else if (ret == 1) {
        val |= (1 << 4);
    }
    writel(val, gpio_dr_va);


    return len;
}

static long led_dev_ioctl(struct file *filep, unsigned int cmd, unsigned long para)
{
    int sw = -1;
    int val;
    
    printk(KERN_NOTICE "ioctl\r\n");

    switch (cmd)
    {
    case LED_ON_CMD:
        printk(KERN_NOTICE "LED_ON_CMD, para: %d\r\n", (int)para);
        sw = 1;
        break;
    case LED_OFF_CMD:
        printk(KERN_NOTICE "LED_OFF_CMD, para: %d\r\n", (int)para);
        sw = 0;
        break;
    case LED_TOGGLE_CMD:
        printk(KERN_NOTICE "LED_TOGGLE_CMD, para: %d\r\n", (int)para);
        break; 
    default:
        printk(KERN_NOTICE "invalid cmd, para: %d\r\n", (int)para);
        return EINVAL;
    }

    val = readl(gpio_dr_va);
    val &= ~(1 << 4);
    if (sw == 0 || sw == 1) {
        if (sw == 1) {
            val |= (1 << 4);
        } else if (sw == 0) {
            val |= (0 << 4);
        }
    }
    writel(val, gpio_dr_va);

    return 0;
}

module_init(platform_driver_led_init);
module_exit(platform_driver_led_exit);