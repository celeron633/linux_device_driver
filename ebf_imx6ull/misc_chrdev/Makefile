KERNELDIR := /home/dengxh/ebf_linux_kernel
CURRENT_PATH := $(shell pwd)

obj-m := misc_chrdev.o

build: kernel_modules

kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean
