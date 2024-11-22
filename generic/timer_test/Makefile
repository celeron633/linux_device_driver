KERNELDIR := /home/dengxh/linux-6.4.0-150600.23.25
CURRENT_PATH := $(shell pwd)
CONFIG_MODULE_SIG=n

obj-m := timer_test.o

build: kernel_modules

kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean