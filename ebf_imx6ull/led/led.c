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
static led_dev_t led_dev_objs[] = {
    // R
    {   
        .dev_pa.CCM_CCGR1_PA = 0x020C406C,
        .dev_pa.IOMUXC_SW_MUX_CTL_PAD_PA = 0x020E006C,
        .dev_pa.IOMUXC_SW_PAD_CTL_PAD_PA = 0x020E02F8,
        .dev_pa.GPIOX_GDIR_PA = 0x0209C004,
        .dev_pa.GPIOX_DR_PA = 0x0209C000
    },
    {
        .dev_pa.CCM_CCGR1_PA = 0x20C4074,
        .dev_pa.IOMUXC_SW_MUX_CTL_PAD_PA = 0x020E01E0,
        .dev_pa.IOMUXC_SW_PAD_CTL_PAD_PA = 0x020E046C,
        .dev_pa.GPIOX_GDIR_PA = 0x020A8004,
        .dev_pa.GPIOX_DR_PA = 0x020A8000
    },
    {
        .dev_pa.CCM_CCGR1_PA = 0x20C4074,
        .dev_pa.IOMUXC_SW_MUX_CTL_PAD_PA = 0x20E01DC,
        .dev_pa.IOMUXC_SW_PAD_CTL_PAD_PA = 0x20E0468,
        .dev_pa.GPIOX_GDIR_PA = 0x20A8004,
        .dev_pa.GPIOX_DR_PA = 0x20A8000
    },
};

static int led_dev_init(void)
{
    u32 val = 0;
    int i = 0;
    struct class *led_class;
    printk(KERN_NOTICE "init");

    // if already with major
    // MKDEV(led_dev_o.dev_major, led_dev_o.dev_minor);

    for (i = 0; i < 3; i++) {
        // phy reg addr to virtual addr
        led_dev_objs[i].dev_va.CCM_CCGR1_VA = ioremap(led_dev_objs[i].dev_pa.CCM_CCGR1_PA, 4);
        led_dev_objs[i].dev_va.IOMUXC_SW_MUX_CTL_PAD_VA = ioremap(led_dev_objs[i].dev_pa.IOMUXC_SW_MUX_CTL_PAD_PA, 4);
        led_dev_objs[i].dev_va.IOMUXC_SW_PAD_CTL_PAD_VA = ioremap(led_dev_objs[i].dev_pa.IOMUXC_SW_PAD_CTL_PAD_PA, 4);
        led_dev_objs[i].dev_va.GPIOX_DR_VA = ioremap(led_dev_objs[i].dev_pa.GPIOX_DR_PA, 4);
        led_dev_objs[i].dev_va.GPIOX_GDIR_VA = ioremap(led_dev_objs[i].dev_pa.GPIOX_GDIR_PA, 4);

        // init led on GPIO1.IO04 manually begin
        // CCM
        val = readl(led_dev_objs[i].dev_va.CCM_CCGR1_VA);
        // reset original value
        val &= ~(3 << 26);
        // write desired value
        val |= (3 << 26);
        writel(val, led_dev_objs[i].dev_va.CCM_CCGR1_VA);

        // MUX
        writel(5, led_dev_objs[i].dev_va.IOMUXC_SW_MUX_CTL_PAD_VA);
        // PAD
        writel(0x10B0, led_dev_objs[i].dev_va.IOMUXC_SW_PAD_CTL_PAD_VA);

        // GDIR (set input or output, set to 1 as output)
        val = readl(led_dev_objs[i].dev_va.GPIOX_GDIR_VA);
        val &= ~(1 << 4);
        val |= (1 << 4);
        writel(val, led_dev_objs[i].dev_va.GPIOX_GDIR_VA);
        
        // DR (data register)
        val = readl(led_dev_objs[i].dev_va.GPIOX_DR_VA);
        val |= (1 << 4);
        writel(val, led_dev_objs[i].dev_va.GPIOX_DR_VA);
        // init led on GPIO1.IO04 manually end

        // assign dev_t
        alloc_chrdev_region(&(led_dev_objs[i].dev_id), 0, 1, LED_DEV_NAME);

        // cdev
        cdev_init(&(led_dev_objs[i].dev_cdev), &led_dev_fops);
        cdev_add(&(led_dev_objs[i].dev_cdev), led_dev_objs[i].dev_id, 1);
    }

    // class & device
    led_class = class_create(THIS_MODULE, LED_DEV_NAME);
    for (i = 0; i < 3; i++) {
        led_dev_objs[i].dev_class = led_class;
        led_dev_objs[i].dev_device = device_create(led_dev_objs[i].dev_class, NULL, led_dev_objs[i].dev_id, NULL, "led1.%d", (i+1));
    }

    return 0;
}

static void led_dev_exit(void)
{
    int i = 0;
    printk(KERN_NOTICE "exit");

    for (i = 0; i < 3; i++) {
        // cdev
        cdev_del(&(led_dev_objs[i].dev_cdev));

        // led_dev_o.dev_class->devnode

        // udev
        unregister_chrdev_region(led_dev_objs[i].dev_id, 1);

        // io mapping release
        iounmap(led_dev_objs[i].dev_va.CCM_CCGR1_VA);
        iounmap(led_dev_objs[i].dev_va.GPIOX_DR_VA);
        iounmap(led_dev_objs[i].dev_va.GPIOX_GDIR_VA);
        iounmap(led_dev_objs[i].dev_va.IOMUXC_SW_MUX_CTL_PAD_VA);
        iounmap(led_dev_objs[i].dev_va.IOMUXC_SW_PAD_CTL_PAD_VA);
    }

    // device & class
    for (i = 0; i < 3; i++) {
        device_destroy(led_dev_objs[i].dev_class, led_dev_objs[i].dev_id);
    }
    class_destroy(led_dev_objs[0].dev_class);
}

static int led_dev_open(struct inode *inodep, struct file *filep)
{
    struct led_dev_t *led_dev_o;

    printk(KERN_NOTICE "open");

    led_dev_o = (struct led_dev_t *)container_of(inodep->i_cdev, struct led_dev_t, dev_cdev);
    filep->private_data = (void *)led_dev_o;

    return 0;
}

static int led_dev_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_NOTICE "release");

    return 0;
}

static ssize_t led_dev_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    char msg_to_user[] = "hello wrorld\n";
    int ret;

    printk(KERN_NOTICE "read");

    ret = copy_to_user(buf, msg_to_user, sizeof(msg_to_user));
    if (ret < 0) {
        printk(KERN_ERR "copy_to_user failed!");
        return EFAULT;
    }
    
    return sizeof(msg_to_user);
}

static ssize_t led_dev_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
    char buffer[256];
    int ret;
    int val;
    struct led_dev_t *led_dev_o;

    printk(KERN_NOTICE "write");

    ret = copy_from_user(buffer, buf, len);
    if (ret < 0) {
        printk(KERN_ERR "copy_from_user failed!");
        return EFAULT;
    }
    printk(KERN_NOTICE "read %d bytes from userspace: [%s]", (int)len, buffer);

    if (strlen(buffer) > 1) {
        ret = buffer[0] - '0';
    }

    // DR (data register)
    led_dev_o = (struct led_dev_t *)f->private_data;
    val = readl(led_dev_o->dev_va.GPIOX_DR_VA);
    val &= ~(1 << 4);
    if (ret == 0) {
        val |= (0 << 4);
    } else if (ret == 1) {
        val |= (1 << 4);
    }
    writel(val, led_dev_o->dev_va.GPIOX_DR_VA);


    return len;
}

static long led_dev_ioctl(struct file *filep, unsigned int cmd, unsigned long para)
{
    int sw = -1;
    int val;
    struct led_dev_t *led_dev_o;
    
    printk(KERN_NOTICE "ioctl");

    switch (cmd)
    {
    case LED_ON_CMD:
        printk(KERN_NOTICE "LED_ON_CMD, para: %d", (int)para);
        sw = 1;
        break;
    case LED_OFF_CMD:
        printk(KERN_NOTICE "LED_OFF_CMD, para: %d", (int)para);
        sw = 0;
        break;
    case LED_TOGGLE_CMD:
        printk(KERN_NOTICE "LED_TOGGLE_CMD, para: %d", (int)para);
        break; 
    default:
        printk(KERN_NOTICE "invalid cmd, para: %d", (int)para);
        return EINVAL;
    }

    led_dev_o = (struct led_dev_t *)filep->private_data;
    val = readl(led_dev_o->dev_va.GPIOX_DR_VA);
    val &= ~(1 << 4);
    if (sw == 0 || sw == 1) {
        if (sw == 1) {
            val |= (1 << 4);
        } else if (sw == 0) {
            val |= (0 << 4);
        }
    }
    writel(val, led_dev_o->dev_va.GPIOX_DR_VA);

    return 0;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dengxh");

module_init(led_dev_init);
module_exit(led_dev_exit);