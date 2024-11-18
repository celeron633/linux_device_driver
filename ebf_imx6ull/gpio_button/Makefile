KERNELDIR := /home/dengxh/ebf_linux_kernel
CURRENT_PATH := $(shell pwd)
CONFIG_MODULE_SIG=n

obj-m := gpio_button.o

build: kernel_modules

kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean