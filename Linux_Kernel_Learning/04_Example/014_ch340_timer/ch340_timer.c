//#include <linux/kernel.h>
//#include <linux/module.h>
//#include <linux/usb.h>
//#include <linux/slab.h>
//#include <linux/timer.h>
//
//#define VENDOR_ID  0x1a86
//#define PRODUCT_ID 0x7523
//
//// CH340 typical bulk endpoints (verify with lsusb -v)
//#define EP_IN  0x82  // Bulk IN endpoint
//#define EP_OUT 0x02  // Bulk OUT endpoint
//
//struct ch340_data {
//    struct usb_device *udev;
//    struct timer_list timer;
//    unsigned char *buffer;
//};
//
//static struct usb_device_id ch340_table[] = {
//    { USB_DEVICE(VENDOR_ID, PRODUCT_ID) },
//    { }
//};
//MODULE_DEVICE_TABLE(usb, ch340_table);
//
//static void send_hello(struct timer_list *t)
//{
//    struct ch340_data *data = from_timer(data, t, timer);
//    int actual_length;
//    char *msg = "hello\r\n";
//    int msg_len = strlen(msg);
//    int ret;
//
//    memcpy(data->buffer, msg, msg_len);
//    
//    // Send data via bulk OUT endpoint
//    ret = usb_bulk_msg(data->udev,
//                       usb_sndbulkpipe(data->udev, EP_OUT),
//                       data->buffer,
//                       msg_len,
//                       &actual_length,
//                       1000);  // 1 second timeout
//                       
//    if (ret) {
//        pr_err("CH340: Bulk write failed: %d\n", ret);
//    } else {
//        pr_info("CH340: Sent %d bytes: %s\n", actual_length, msg);
//    }
//
//    // Reschedule timer for 10 seconds
//    mod_timer(&data->timer, jiffies + msecs_to_jiffies(10000));
//}
//
//static int ch340_probe(struct usb_interface *interface, const struct usb_device_id *id)
//{
//    struct usb_device *udev = interface_to_usbdev(interface);
//    struct ch340_data *data;
//    int ret;
//
//    pr_info("CH340: Device plugged (Vendor=0x%04X, Product=0x%04X)\n",
//            id->idVendor, id->idProduct);
//
//    data = kzalloc(sizeof(*data), GFP_KERNEL);
//    if (!data)
//        return -ENOMEM;
//
//    data->udev = udev;
//    data->buffer = kmalloc(256, GFP_KERNEL);
//    if (!data->buffer) {
//        kfree(data);
//        return -ENOMEM;
//    }
//
//    usb_set_intfdata(interface, data);
//
//    // Initialize and start the timer
//    timer_setup(&data->timer, send_hello, 0);
//    mod_timer(&data->timer, jiffies + msecs_to_jiffies(10000));
//
//    pr_info("CH340: Timer started, will send data every 10 seconds\n");
//    return 0;
//}
//
//static void ch340_disconnect(struct usb_interface *interface)
//{
//    struct ch340_data *data = usb_get_intfdata(interface);
//
//    pr_info("CH340: Device unplugged\n");
//
//    del_timer_sync(&data->timer);
//    kfree(data->buffer);
//    kfree(data);
//}
//
//static struct usb_driver ch340_driver = {
//    .name       = "ch340_timer",
//    .id_table   = ch340_table,
//    .probe      = ch340_probe,
//    .disconnect = ch340_disconnect,
//};
//
//module_usb_driver(ch340_driver);
//
//MODULE_LICENSE("GPL");
//MODULE_AUTHOR("Your Name");
//MODULE_DESCRIPTION("CH340 Periodic Data Sender");
//

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

int main() {
    int fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror("open failed");
        return -1;
    }

    struct termios tty;
    tcgetattr(fd, &tty);

    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;

    tcsetattr(fd, TCSANOW, &tty);

    while (1) {
        write(fd, "hello\n", 6);
        sleep(10);
    }

    close(fd);
    return 0;
}
