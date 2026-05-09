# Makefile for msgdev kernel module

MODULE := msgdev
KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

obj-m := $(MODULE).o

# Build the module
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# Clean build artifacts
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

# Insert module
install: all
	sudo insmod $(MODULE).ko || true

# Remove module
remove:
	sudo rmmod $(MODULE) || true

# Reload module (remove + install)
reload: remove install

# View recent kernel messages related to module
logs:
	sudo dmesg -T | tail -n 50

# Follow kernel messages in real-time
logs-follow:
	sudo dmesg -w
