#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define DEVICE_NAME "usb829"
#define CLASS_NAME  "usb829_class"
#define BUF_SIZE    256

static dev_t dev_num;
static struct cdev usb829_cdev;
static struct class *usb829_class = NULL;
static struct device *usb829_device = NULL;

static char buffer[BUF_SIZE];
static int data_size = 0;

static char *usb829_devnode(const struct device *dev, umode_t *mode)
{
    if (mode)
        *mode = 0666;
    return NULL;
}

/* ================= OPEN ================= */
static int usb829_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "usb829: device opened\n");
    return 0;
}

/* ================= RELEASE ================= */
static int usb829_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "usb829: device closed\n");
    return 0;
}

/* ================= WRITE ================= */
/* user -> kernel */
static ssize_t usb829_write(struct file *file,
                           const char __user *buf,
                           size_t len,
                           loff_t *off)
{
    char kbuf[BUF_SIZE];

    if (len > BUF_SIZE - 6)  // reserve space for "ACK: "
        len = BUF_SIZE - 6;

    if (copy_from_user(kbuf, buf, len))
        return -EFAULT;

    /* build ACK response */
    snprintf(buffer, BUF_SIZE, "ACK: %.*s", (int)len, kbuf);
    data_size = strlen(buffer);

    printk(KERN_INFO "usb829: received %zu bytes\n", len);

    return len;
}

/* ================= READ ================= */
/* kernel -> user */
static ssize_t usb829_read(struct file *file,
                          char __user *buf,
                          size_t len,
                          loff_t *off)
{
    if (*off >= data_size)
        return 0;  // EOF

    if (len > data_size - *off)
        len = data_size - *off;

    if (copy_to_user(buf, buffer + *off, len))
        return -EFAULT;

    *off += len;
    return len;
}

/* ================= FILE OPS ================= */
static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = usb829_open,
    .release = usb829_release,
    .read    = usb829_read,
    .write   = usb829_write,
};

/* ================= INIT ================= */
static int __init virtual_char_device_init(void)
{
    int ret;

    /* allocate device number */
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        printk(KERN_ERR "usb829: failed to allocate chrdev\n");
        return ret;
    }

    /* init cdev */
    cdev_init(&usb829_cdev, &fops);
    ret = cdev_add(&usb829_cdev, dev_num, 1);
    if (ret < 0) {
        unregister_chrdev_region(dev_num, 1);
        return ret;
    }

    /* create class */
    usb829_class = class_create(CLASS_NAME);
    if (IS_ERR(usb829_class)) {
        cdev_del(&usb829_cdev);
        unregister_chrdev_region(dev_num, 1);
        return PTR_ERR(usb829_class);
    }

    /* ADD THIS */
    usb829_class->devnode = usb829_devnode;

    /* create device node automatically */
    usb829_device = device_create(usb829_class, NULL, dev_num, NULL, DEVICE_NAME);
    if (IS_ERR(usb829_device)) {
        class_destroy(usb829_class);
        cdev_del(&usb829_cdev);
        unregister_chrdev_region(dev_num, 1);
        return PTR_ERR(usb829_device);
    }

    printk(KERN_INFO "usb829: module loaded\n");
    printk(KERN_INFO "usb829: /dev/%s created\n", DEVICE_NAME);

    return 0;
}


/* ================= EXIT ================= */
static void __exit virtual_char_device_exit(void)
{
    device_destroy(usb829_class, dev_num);
    class_destroy(usb829_class);
    cdev_del(&usb829_cdev);
    unregister_chrdev_region(dev_num, 1);

    printk(KERN_INFO "usb829: module unloaded\n");
}

module_init(virtual_char_device_init);
module_exit(virtual_char_device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shirley Ra");
MODULE_DESCRIPTION("Virtual char device with ACK response");
