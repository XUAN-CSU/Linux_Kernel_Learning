#ifndef MSG_DEVICE_H
#define MSG_DEVICE_H

#include <linux/ioctl.h>

/* ioctl commands */
#define MSGDEV_IOC_MAGIC 'k'
#define MSGDEV_IOC_CLEAR  _IO(MSGDEV_IOC_MAGIC, 0)
#define MSGDEV_IOC_GETLEN _IOR(MSGDEV_IOC_MAGIC, 1, int)
#define MSGDEV_IOC_MAXNR 1

#endif /* MSG_DEVICE_H */
