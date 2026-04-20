obj-m += register_char_device.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)
MODULE := register_char_device

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

install: all
	sudo insmod $(MODULE).ko || true

clean_devices:
	@echo "Cleaning up device files..."
	@sudo rm -f /dev/eep-mem* 2>/dev/null || true

remove: clean_devices
	sudo rmmod $(MODULE) || true

reload: remove install

logs:
	sudo dmesg -T | tail -n 50

logs-follow:
	sudo dmesg -w
