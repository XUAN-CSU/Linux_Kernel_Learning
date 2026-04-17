## Makefile for ch340_timer kernel module
#
## Module name
#obj-m += ch340_timer.o
#
## Kernel build directory
#KERNEL_DIR := /lib/modules/$(shell uname -r)/build
#
## Current directory
#PWD := $(shell pwd)
#
## Module name (without .o)
#MODULE_NAME := ch340_timer
#
## Colors for output
#GREEN  := \033[0;32m
#RED    := \033[0;31m
#YELLOW := \033[0;33m
#NC     := \033[0m # No Color
#
#.PHONY: all clean install remove logs dmesg info check reload help
#
#all: check_modules
#	@echo "$(GREEN)Building $(MODULE_NAME) module...$(NC)"
#	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules
#	@echo "$(GREEN)Build complete!$(NC)"
#
#clean:
#	@echo "$(YELLOW)Cleaning build files...$(NC)"
#	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean
#	@rm -f *.order *.symvers *.mod
#	@echo "$(GREEN)Clean complete!$(NC)"
#
#install: all
#	@echo "$(YELLOW)Checking for conflicting drivers...$(NC)"
#	@if lsmod | grep -q "^ch341"; then \
#		echo "$(RED)Warning: ch341 driver is loaded. Attempting to remove...$(NC)"; \
#		sudo rmmod ch341 2>/dev/null || true; \
#	fi
#	@if lsmod | grep -q "^usbserial"; then \
#		echo "$(YELLOW)Removing usbserial...$(NC)"; \
#		sudo rmmod usbserial 2>/dev/null || true; \
#	fi
#	@echo "$(GREEN)Installing $(MODULE_NAME) module...$(NC)"
#	sudo insmod $(MODULE_NAME).ko
#	@echo "$(GREEN)Module installed successfully!$(NC)"
#	@echo "$(YELLOW)Module info:$(NC)"
#	@lsmod | grep $(MODULE_NAME) || echo "$(RED)Module not found in lsmod$(NC)"
#	@echo "$(YELLOW)Kernel messages:$(NC)"
#	@dmesg | tail -5
#
#remove:
#	@echo "$(YELLOW)Removing $(MODULE_NAME) module...$(NC)"
#	@if lsmod | grep -q "^$(MODULE_NAME)"; then \
#		sudo rmmod $(MODULE_NAME); \
#		echo "$(GREEN)Module removed!$(NC)"; \
#	else \
#		echo "$(RED)Module $(MODULE_NAME) is not loaded$(NC)"; \
#	fi
#
#reload: remove install
#	@echo "$(GREEN)Module reloaded!$(NC)"
#
#logs:
#	@echo "$(YELLOW)Tailing kernel log (Ctrl+C to exit)...$(NC)"
#	sudo dmesg -w | grep -E "$(MODULE_NAME)|CH340|usb|ch341" --color=always
#
#dmesg:
#	@echo "$(YELLOW)Recent kernel messages related to USB/CH340:$(NC)"
#	@dmesg | grep -E "$(MODULE_NAME)|CH340|ch341|usb.*1a86|ttyUSB" --color=always | tail -30
#
#info:
#	@echo "$(GREEN)=== Build Information ===$(NC)"
#	@echo "Kernel Version: $(shell uname -r)"
#	@echo "Kernel Directory: $(KERNEL_DIR)"
#	@echo "Module Name: $(MODULE_NAME)"
#	@echo "Current Directory: $(PWD)"
#	@echo ""
#	@echo "$(GREEN)=== Loaded USB/Serial Modules ===$(NC)"
#	@lsmod | grep -E "Module|usb|ch34|serial" --color=always || echo "No relevant modules loaded"
#	@echo ""
#	@echo "$(GREEN)=== CH340 Devices ===$(NC)"
#	@lsusb | grep -i "1a86:7523" || echo "No CH340 device found"
#	@echo ""
#	@echo "$(GREEN)=== Serial Ports ===$(NC)"
#	@ls -la /dev/ttyUSB* 2>/dev/null || echo "No /dev/ttyUSB* devices found"
#
#check_modules:
#	@echo "$(YELLOW)Checking kernel module dependencies...$(NC)"
#	@if [ ! -d "$(KERNEL_DIR)" ]; then \
#		echo "$(RED)Error: Kernel headers not found at $(KERNEL_DIR)$(NC)"; \
#		echo "$(YELLOW)Install them with: sudo apt install linux-headers-$(shell uname -r)$(NC)"; \
#		exit 1; \
#	fi
#
#check_device:
#	@echo "$(YELLOW)Checking for CH340 device...$(NC)"
#	@if lsusb | grep -q "1a86:7523"; then \
#		echo "$(GREEN)CH340 device found!$(NC)"; \
#		lsusb -v -d 1a86:7523 2>/dev/null | grep -E "idVendor|idProduct|bEndpointAddress" || true; \
#	else \
#		echo "$(RED)No CH340 device detected. Plug in the device first.$(NC)"; \
#	fi
#
#test_write:
#	@echo "$(YELLOW)Sending test data to CH340...$(NC)"
#	@if [ -c /dev/ttyUSB0 ]; then \
#		echo -e "hello\r\n" | sudo tee /dev/ttyUSB0 > /dev/null; \
#		echo "$(GREEN)Test message sent!$(NC)"; \
#	else \
#		echo "$(RED)/dev/ttyUSB0 not found. Is the device connected?$(NC)"; \
#	fi
#
#help:
#	@echo "$(GREEN)Available targets:$(NC)"
#	@echo "  make            - Build the kernel module"
#	@echo "  make clean      - Remove build artifacts"
#	@echo "  make install    - Build and insert the module (removes conflicts)"
#	@echo "  make remove     - Remove the module from kernel"
#	@echo "  make reload     - Remove and reinstall the module"
#	@echo "  make logs       - Watch kernel logs in real-time (filtered)"
#	@echo "  make dmesg      - Show recent USB/CH340 kernel messages"
#	@echo "  make info       - Display system and module information"
#	@echo "  make check_device - Check if CH340 is connected"
#	@echo "  make test_write - Send test message to /dev/ttyUSB0"
#	@echo "  make help       - Show this help message"
