#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include "msg_device.h"

int main()
{
    int fd, len;
    char write_buf[] = "Hello from userspace!";
    char read_buf[256] = {0};
    
    /* Open the device */
    fd = open("/dev/msgdev", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }
    
    printf("Device opened successfully\n");
    
    /* Write data to the device */
    printf("Writing: %s\n", write_buf);
    if (write(fd, write_buf, strlen(write_buf)) < 0) {
        perror("Write failed");
        close(fd);
        return 1;
    }
    
    /* Get message length via ioctl */
    if (ioctl(fd, MSGDEV_IOC_GETLEN, &len) < 0) {
        perror("IOCTL GETLEN failed");
        close(fd);
        return 1;
    }
    printf("Message length: %d\n", len);
    
    /* Read data back */
    if (read(fd, read_buf, len) < 0) {
        perror("Read failed");
        close(fd);
        return 1;
    }
    printf("Read back: %s\n", read_buf);
    
    /* Clear the message via ioctl */
    printf("Clearing message with ioctl\n");
    if (ioctl(fd, MSGDEV_IOC_CLEAR) < 0) {
        perror("IOCTL CLEAR failed");
        close(fd);
        return 1;
    }
    
    /* Verify it's cleared */
    if (ioctl(fd, MSGDEV_IOC_GETLEN, &len) < 0) {
        perror("IOCTL GETLEN failed");
        close(fd);
        return 1;
    }
    printf("After clear - message length: %d\n", len);
    
    close(fd);
    printf("Device closed\n");
    return 0;
}
