#include "led.h"

static int led_dev_init(void);
static void led_dev_exit(void);
static int led_dev_open(struct inode *, struct file *);
static int led_dev_release(struct inode *, struct file *);
static ssize_t led_dev_read(struct file *f, char __user *buf, size_t len, loff_t *off);
static ssize_t led_dev_write(struct file *f, const char __user *buf, size_t len, loff_t *off);
static long led_dev_ioctl(struct file *filep, unsigned int cmd, unsigned long para);

struct file_operations led_dev_fops = {
    .open = led_dev_open,
    .release = led_dev_release,
    .read = led_dev_read,
    .write = led_dev_write,
    .compat_ioctl = led_dev_ioctl,
    .unlocked_ioctl = led_dev_ioctl
};

// instance
static led_dev_t led_object;

static int led_dev_init(void)
{
    u32 val = 0;

    printk(KERN_NOTICE "init\r\n");

    // if already with major
    // MKDEV(led_object.dev_major, led_object.dev_minor);

    // phy reg addr to virtual addr
    led_object.dev_regs.CCM_CCGR1_VA = ioremap(CCM_CCGR1_PHY, 4);
    led_object.dev_regs.IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO04_VA = ioremap(IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO04_PHY, 4);
    led_object.dev_regs.IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO04_VA = ioremap(IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO04_PHY, 4);
    led_object.dev_regs.GPIO1_DR_VA = ioremap(GPIO1_DR_PHY, 4);
    led_object.dev_regs.GPIO1_GDIR_VA = ioremap(GPIO1_GDIR_PHY, 4);

    // init led on GPIO1.IO04 manually begin
    // CCM
    val = readl(led_object.dev_regs.CCM_CCGR1_VA);
    // reset original value
    val &= ~(3 << 26);
    // write desired value
    val |= (3 << 26);
    writel(val, led_object.dev_regs.CCM_CCGR1_VA);

    // MUX
    writel(5, led_object.dev_regs.IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO04_VA);
    // PAD
    writel(0x10B0, led_object.dev_regs.IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO04_VA);

    // GDIR (set input or output, set to 1 as output)
    val = readl(led_object.dev_regs.GPIO1_GDIR_VA);
    val &= ~(1 << 4);
    val |= (1 << 4);
    writel(val, led_object.dev_regs.GPIO1_GDIR_VA);
    
    // DR (data register)
    val = readl(led_object.dev_regs.GPIO1_DR_VA);
    val |= (1 << 4);
    writel(val, led_object.dev_regs.GPIO1_DR_VA);

    // init led on GPIO1.IO04 manually end

    // assign dev_t
    alloc_chrdev_region(&(led_object.dev_id), 0, 1, LED_DEV_NAME);

    // cdev
    cdev_init(&(led_object.dev_cdev), &led_dev_fops);
    cdev_add(&(led_object.dev_cdev), led_object.dev_id, 1);
    
    // class & device
    led_object.dev_class = class_create(THIS_MODULE, LED_DEV_NAME);
    led_object.dev_device = device_create(led_object.dev_class, NULL, led_object.dev_id, NULL, LED_DEV_NAME);

    return 0;
}

static void led_dev_exit(void)
{
    printk(KERN_NOTICE "exit\r\n");

    // cdev
    cdev_del(&(led_object.dev_cdev));

    // device & class
    device_destroy(led_object.dev_class, led_object.dev_id);
    class_destroy(led_object.dev_class);

    // led_object.dev_class->devnode

    // udev
    unregister_chrdev_region(led_object.dev_id, 1);

    // io mapping release
    iounmap(led_object.dev_regs.CCM_CCGR1_VA);
    iounmap(led_object.dev_regs.GPIO1_DR_VA);
    iounmap(led_object.dev_regs.GPIO1_GDIR_VA);
    iounmap(led_object.dev_regs.IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO04_VA);
    iounmap(led_object.dev_regs.IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO04_VA);
}

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
    char msg_to_user[] = "hello wrorld\n";
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
    val = readl(led_object.dev_regs.GPIO1_DR_VA);
    val &= ~(1 << 4);
    if (ret == 0) {
        val |= (0 << 4);
    } else if (ret == 1) {
        val |= (1 << 4);
    }
    writel(val, led_object.dev_regs.GPIO1_DR_VA);

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

    val = readl(led_object.dev_regs.GPIO1_DR_VA);
    val &= ~(1 << 4);
    if (sw == 0 || sw == 1) {
        if (sw == 1) {
            val |= (1 << 4);
        } else if (sw == 0) {
            val |= (0 << 4);
        }
    }
    writel(val, led_object.dev_regs.GPIO1_DR_VA);

    return 0;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dengxh");

module_init(led_dev_init);
module_exit(led_dev_exit);