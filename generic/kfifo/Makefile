KERNELDIR := /home/dengxh/linux_kernel_dev/linux-4.19.324
CURRENT_PATH := $(shell pwd)
CONFIG_MODULE_SIG=n

obj-m := kfifo1.o

build: kernel_modules

kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean
