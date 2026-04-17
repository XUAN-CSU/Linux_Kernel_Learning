# Module name
obj-m += virtual_char_device.o

# Kernel build directory
KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

DEVICE := /dev/usb829

# ================= BUILD =================
all:
	make -C $(KDIR) M=$(PWD) modules

# ================= CLEAN =================
clean:
	make -C $(KDIR) M=$(PWD) clean

# ================= INSTALL =================
install:
	sudo insmod virtual_char_device.ko || true
	@echo "Module inserted"
	@ls -l $(DEVICE) || echo "Device not created!"

# ================= REMOVE =================
remove:
	sudo rmmod virtual_char_device || true
	@echo "Module removed"

# ================= INFO =================
info:
	@echo "===== Module Info ====="
	@lsmod | grep virtual_char_device || echo "Not loaded"
	@echo
	@echo "===== Device ====="
	@ls -l $(DEVICE) || echo "Device missing"
	@echo
	@echo "===== dmesg (last 20) ====="
	@dmesg | tail -n 20

# ================= LOGS =================
logs:
	sudo dmesg -wT | grep virtual_char_device

# ================= RUN (interactive) =================
run:
	@echo "Type something → will send to /dev/usb829"
	@echo "Press Ctrl+C to exit"
	@while true; do \
		read -p ">> " input; \
		echo $$input > $(DEVICE); \
		# echo -n "Response: "; \
		cat $(DEVICE); \
		echo; \
	done

# ================= RUN TIMER =================
run_timer:
	@echo "Sending 'Hello, Shirley' every 5 seconds..."
	@echo "Press Ctrl+C to stop"
	@while true; do \
		echo "Hello, Shirley" > $(DEVICE); \
		# echo -n "Response: "; \
		cat $(DEVICE); \
		echo; \
		sleep 5; \
	done
