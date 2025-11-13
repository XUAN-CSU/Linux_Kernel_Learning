/* usb_notifier_example.c
 *
 * Monitor USB device insert/remove events via usb_register_notify()
 *
 * Build: use normal external module Makefile
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/usb.h>
#include <linux/notifier.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("chatgpt");
MODULE_DESCRIPTION("USB insert/remove monitor (notifier example)");

static int usb_event_notify(struct notifier_block *nb, unsigned long action, void *data)
{
    struct usb_device *udev = data;

    /* data may be NULL depending on action; check */
    switch (action) {
    case USB_DEVICE_ADD:
        if (udev) {
            pr_info("WLC USB_MON: Device added: vendor=0x%04x prod=0x%04x devnum=%u\n",
                    le16_to_cpu(udev->descriptor.idVendor),
                    le16_to_cpu(udev->descriptor.idProduct),
                    udev->devnum);
        } else {
            pr_info("WLC USB_MON: USB_DEVICE_ADD (no device ptr)\n");
        }
        break;
    case USB_DEVICE_REMOVE:
        if (udev) {
            pr_info("WLC USB_MON: Device removed: vendor=0x%04x prod=0x%04x devnum=%u\n",
                    le16_to_cpu(udev->descriptor.idVendor),
                    le16_to_cpu(udev->descriptor.idProduct),
                    udev->devnum);
        } else {
            pr_info("WLC USB_MON: USB_DEVICE_REMOVE (no device ptr)\n");
        }
        break;
    case USB_BUS_ADD:
        pr_info("WLC USB_MON: USB bus added\n");
        break;
    case USB_BUS_REMOVE:
        pr_info("WLC USB_MON: USB bus removed\n");
        break;
    default:
        pr_info("WLC USB_MON: action=%lu\n", action);
        break;
    }

    return NOTIFY_OK;
}

/* notifier_block struct instance */
static struct notifier_block usb_nb = {
    .notifier_call = usb_event_notify,
};

static int __init tasklet_usb_monitoring_example_init(void)
{
    pr_info("WLC USB_MON: init\n");
    usb_register_notify(&usb_nb);

    pr_info("WLC USB_MON: notifier registered\n");
    return 0;
}

static void __exit tasklet_usb_monitoring_example_exit(void)
{
    pr_info("WLC USB_MON: exit\n");
    usb_unregister_notify(&usb_nb);
    pr_info("WLC USB_MON: notifier unregistered\n");
}

module_init(tasklet_usb_monitoring_example_init);
module_exit(tasklet_usb_monitoring_example_exit);
