#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define EEP_NBANK 8
#define EEP_DEVICE_NAME "eep-mem"
#define EEP_CLASS "eep-class"

struct class *eep_class;
struct cdev eep_cdev[EEP_NBANK];
dev_t dev_num;


static int eep_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int eep_release(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t eep_read(struct file *file, char __user *buf,
                        size_t count, loff_t *ppos)
{
    return 0;
}

static ssize_t eep_write(struct file *file, const char __user *buf,
                         size_t count, loff_t *ppos)
{
    return count;
}

static const struct file_operations eep_fops = {
    .owner   = THIS_MODULE,
    .open    = eep_open,
    .release = eep_release,
    .read    = eep_read,
    .write   = eep_write,
    // .llseek, .ioctl, etc. if needed
};

static int __init register_char_device_init(void)
{
    int i, ret;
    dev_t curr_dev;

    ret = alloc_chrdev_region(&dev_num, 0, EEP_NBANK, EEP_DEVICE_NAME);
    if (ret < 0) {
        pr_err("Failed to allocate chrdev region\n");
        return ret;
    }

    eep_class = class_create(EEP_CLASS);
    if (IS_ERR(eep_class)) {
        pr_err("Failed to create class\n");
        unregister_chrdev_region(dev_num, EEP_NBANK);
        return PTR_ERR(eep_class);
    }

    for (i = 0; i < EEP_NBANK; ++i) {
        cdev_init(&eep_cdev[i], &eep_fops);
        eep_cdev[i].owner = THIS_MODULE;
        
        curr_dev = MKDEV(MAJOR(dev_num), MINOR(dev_num) + i);

        ret = cdev_add(&eep_cdev[i], curr_dev, 1);
        if (ret < 0) {
            pr_err("Failed to add cdev for bank %d\n", i);
            goto err_cdev_add;
        }

        if (device_create(eep_class, NULL, curr_dev, NULL,
                          EEP_DEVICE_NAME "%d", i) == NULL) {
            pr_err("Failed to create device for bank %d\n", i);
            cdev_del(&eep_cdev[i]);
            goto err_cdev_add;
        }
    }

    pr_info("WLC load\n");

    return 0;

err_cdev_add:
    /* Roll back all previously added cdevs */
    for (--i; i >= 0; --i) {
        curr_dev = MKDEV(MAJOR(dev_num), MINOR(dev_num) + i);
        device_destroy(eep_class, curr_dev);
        cdev_del(&eep_cdev[i]);
    }
    class_destroy(eep_class);
    unregister_chrdev_region(dev_num, EEP_NBANK);
    return ret;
}

static void __exit register_char_device_exit(void)
{
    int i;
    dev_t curr_dev;

    for (i = 0; i < EEP_NBANK; i++) {
        curr_dev = MKDEV(MAJOR(dev_num), MINOR(dev_num) + i);

        /* Remove /dev node */
        device_destroy(eep_class, curr_dev);

        /* Remove cdev from kernel */
        cdev_del(&eep_cdev[i]);
    }

    /* Destroy class (/sys/class/eep-class) */
    class_destroy(eep_class);

    /* Release device numbers */
    unregister_chrdev_region(dev_num, EEP_NBANK);

    pr_info("WLC unload\n");
}

module_init(register_char_device_init);
module_exit(register_char_device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shirley Ra");
MODULE_DESCRIPTION("char device register");
