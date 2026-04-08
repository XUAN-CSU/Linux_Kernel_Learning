obj-m += mutex_demo.o

KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean

install:
	sudo insmod mutex_demo.ko

remove:
	sudo rmmod mutex_demo

info:
	modinfo mutex_demo.ko

logs:
	sudo dmesg | tail -20

watch:
	sudo dmesg -w | grep "mutex\|Thread"
