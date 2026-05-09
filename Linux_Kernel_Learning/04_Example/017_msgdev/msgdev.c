#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define DEVICE_NAME "msgdev"
#define CLASS_NAME "msgclass"
#define BUFFER_SIZE 256

/* ioctl commands - must be unique */
#define MSGDEV_IOC_MAGIC 'k'
#define MSGDEV_IOC_CLEAR  _IO(MSGDEV_IOC_MAGIC, 0)
#define MSGDEV_IOC_GETLEN _IOR(MSGDEV_IOC_MAGIC, 1, int)
#define MSGDEV_IOC_MAXNR 1

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shirley Ra");
MODULE_DESCRIPTION("A simple character device with ioctl");

static int major_number;
static struct class *msgdev_class = NULL;
static struct device *msgdev_device = NULL;
static struct cdev msgdev_cdev;

/* Per-device data structure */
struct msg_device_data {
    char message[BUFFER_SIZE];
    int msg_len;
    struct mutex lock;  /* Protect concurrent access */
};

static struct msg_device_data *dev_data;

/* Called when device is opened */
static int dev_open(struct inode *inodep, struct file *filep)
{
    filep->private_data = dev_data;
    pr_info("msgdev: Device opened\n");
    return 0;
}

/* Called when device is closed */
static int dev_release(struct inode *inodep, struct file *filep)
{
    pr_info("msgdev: Device closed\n");
    return 0;
}

/* Read from the device */
static ssize_t dev_read(struct file *filep, char __user *buffer, 
                        size_t len, loff_t *offset)
{
    struct msg_device_data *data = filep->private_data;
    ssize_t retval = 0;
    
    if (mutex_lock_interruptible(&data->lock))
        return -ERESTARTSYS;
    
    /* Check if there's data to read */
    if (*offset >= data->msg_len)
        goto out;  /* EOF */
    
    if (len > data->msg_len - *offset)
        len = data->msg_len - *offset;
    
    /* Copy data to userspace */
    if (copy_to_user(buffer, data->message + *offset, len)) {
        retval = -EFAULT;
        goto out;
    }
    
    *offset += len;
    retval = len;
    
    pr_info("msgdev: Sent %zu bytes to user\n", len);

out:
    mutex_unlock(&data->lock);
    return retval;
}

/* Write to the device */
static ssize_t dev_write(struct file *filep, const char __user *buffer,
                         size_t len, loff_t *offset)
{
    struct msg_device_data *data = filep->private_data;
    
    if (mutex_lock_interruptible(&data->lock))
        return -ERESTARTSYS;
    
    if (len > BUFFER_SIZE - 1)
        len = BUFFER_SIZE - 1;
    
    if (copy_from_user(data->message, buffer, len)) {
        mutex_unlock(&data->lock);
        return -EFAULT;
    }
    
    data->message[len] = '\0';
    data->msg_len = len;
    
    pr_info("msgdev: Received %zu bytes from user\n", len);
    
    mutex_unlock(&data->lock);
    return len;
}

/* ioctl handler - this is where the magic happens */
static long dev_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
    struct msg_device_data *data = filep->private_data;
    int retval = 0;
    int tmp;
    
    /* Validate magic number */
    if (_IOC_TYPE(cmd) != MSGDEV_IOC_MAGIC)
        return -ENOTTY;
    
    /* Validate command number */
    if (_IOC_NR(cmd) > MSGDEV_IOC_MAXNR)
        return -ENOTTY;
    
    if (mutex_lock_interruptible(&data->lock))
        return -ERESTARTSYS;
    
    switch (cmd) {
    case MSGDEV_IOC_CLEAR:
        pr_info("msgdev: IOCTL - Clearing message\n");
        memset(data->message, 0, BUFFER_SIZE);
        data->msg_len = 0;
        break;
        
    case MSGDEV_IOC_GETLEN:
        pr_info("msgdev: IOCTL - Getting message length\n");
        tmp = data->msg_len;
        if (copy_to_user((int __user *)arg, &tmp, sizeof(int))) {
            retval = -EFAULT;
            goto out;
        }
        break;
        
    default:
        retval = -ENOTTY;
    }
    
out:
    mutex_unlock(&data->lock);
    return retval;
}

/* File operations structure */
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write,
    .unlocked_ioctl = dev_ioctl,  /* Modern ioctl interface */
};

/* Module initialization */
static int __init msgdev_init(void)
{
    dev_t dev_num;
    
    pr_info("msgdev: Initializing\n");
    
    /* Allocate device data */
    dev_data = kmalloc(sizeof(struct msg_device_data), GFP_KERNEL);
    if (!dev_data) {
        pr_err("msgdev: Failed to allocate memory\n");
        return -ENOMEM;
    }
    
    /* Initialize device data */
    memset(dev_data->message, 0, BUFFER_SIZE);
    dev_data->msg_len = 0;
    mutex_init(&dev_data->lock);
    
    /* Dynamically allocate a major number */
    if (alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME) < 0) {
        pr_err("msgdev: Failed to allocate major number\n");
        kfree(dev_data);
        return -1;
    }
    major_number = MAJOR(dev_num);
    pr_info("msgdev: Registered with major number %d\n", major_number);
    
    /* Create device class */
    msgdev_class = class_create(CLASS_NAME);
    if (IS_ERR(msgdev_class)) {
        pr_err("msgdev: Failed to create class\n");
        unregister_chrdev_region(MKDEV(major_number, 0), 1);
        kfree(dev_data);
        return PTR_ERR(msgdev_class);
    }
    
    /* Create the device */
    msgdev_device = device_create(msgdev_class, NULL, 
                                  MKDEV(major_number, 0), 
                                  NULL, DEVICE_NAME);
    if (IS_ERR(msgdev_device)) {
        pr_err("msgdev: Failed to create device\n");
        class_destroy(msgdev_class);
        unregister_chrdev_region(MKDEV(major_number, 0), 1);
        kfree(dev_data);
        return PTR_ERR(msgdev_device);
    }
    
    /* Initialize and add cdev */
    cdev_init(&msgdev_cdev, &fops);
    if (cdev_add(&msgdev_cdev, MKDEV(major_number, 0), 1) < 0) {
        pr_err("msgdev: Failed to add cdev\n");
        device_destroy(msgdev_class, MKDEV(major_number, 0));
        class_destroy(msgdev_class);
        unregister_chrdev_region(MKDEV(major_number, 0), 1);
        kfree(dev_data);
        return -1;
    }
    
    pr_info("msgdev: Device created at /dev/%s\n", DEVICE_NAME);
    return 0;
}

/* Module cleanup */
static void __exit msgdev_exit(void)
{
    pr_info("msgdev: Cleaning up\n");
    
    cdev_del(&msgdev_cdev);
    device_destroy(msgdev_class, MKDEV(major_number, 0));
    class_destroy(msgdev_class);
    unregister_chrdev_region(MKDEV(major_number, 0), 1);
    
    mutex_destroy(&dev_data->lock);
    kfree(dev_data);
    
    pr_info("msgdev: Goodbye!\n");
}

module_init(msgdev_init);
module_exit(msgdev_exit);
