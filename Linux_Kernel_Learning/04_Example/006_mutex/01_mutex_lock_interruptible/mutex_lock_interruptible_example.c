#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/delay.h>

#define DEVICE_NAME "mutex_dev"
static int major;
static struct class *cls;
static struct cdev my_cdev;

static DEFINE_MUTEX(my_lock);    // Kernel mutex

static char device_buffer[128] = "Hello from the kernel mutex device!\n";

// File Operation
static ssize_t my_read(struct file *filp, char __user *buf, size_t count, loff_t *ppos)
{
    int ret;

    pr_info("mutex_dev: tring to acquire mutex ...\n");

    ret = mutex_lock_interruptible(&my_lock);
    if (ret) {
        pr_info("mutex_dev: interrupted while waiting for the mutex\n");
        return -EINTR;
    }

    pr_info("mutex_dev: mutex acquired, reading data ...\n");
    ssleep(10);

    if (*ppos >= strlen(device_buffer)) {
        mutex_unlock(&my_lock);
        return 0;   // EOF
    }

    if (count > strlen(device_buffer) - *ppos)
        count = strlen(device_buffer) - *ppos;

    if (copy_to_user(buf, device_buffer + *ppos, count)) {
        mutex_unlock(&my_lock);
        return -EFAULT;
    }

    *ppos += count;

    mutex_unlock(&my_lock);
    return count;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = my_read,
};

static int __init my_module_init(void)
{
    dev_t dev;
    int ret;

    ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (ret < 0) 
        return ret;

    major = MAJOR(dev);

    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE;

    ret = cdev_add(&my_cdev, dev, 1);
    if (ret)
        goto unregister;

    // If your kernel >= 6.4, remove THIS_MODULE
    // cls = class_create(THIS_MODULE, "mutex_class");
    cls = class_create("mutex_class");
    if (IS_ERR(cls)) {
        ret = PTR_ERR(cls);
        goto del_cdev;
    }

    device_create(cls, NULL, dev, NULL, DEVICE_NAME);
    mutex_init(&my_lock);

    pr_info("mutex_dev: module loaded. Major = %d\n", major);
    return 0;

del_cdev:
    cdev_del(&my_cdev);
unregister:
    unregister_chrdev_region(dev, 1);
    return ret;
}

static void __exit my_module_exit(void)
{
    dev_t dev = MKDEV(major, 0);

    device_destroy(cls, dev);
    class_destroy(cls);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev, 1);
    pr_info("mutex dev: module unloaded\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("chatGPT");
MODULE_DESCRIPTION("Example using the mutex_locl_interruptible in kernel");
