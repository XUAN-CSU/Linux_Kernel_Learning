obj-m += tasklet_usb_monitoring_example.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

install:
	sudo insmod tasklet_usb_monitoring_example.ko

remove:
	sudo rmmod tasklet_usb_monitoring_example.ko

info:
	modinfo tasklet_usb_monitoring_example.ko

logs:
	sudo dmesg | tail -20

watch:
	sudo dmesg -w | grep "WLC"

