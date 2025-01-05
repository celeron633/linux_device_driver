KERNELDIR := /home/dengxh/ebf_linux_kernel
CURRENT_PATH := $(shell pwd)
CONFIG_MODULE_SIG=n

CFLAGS_spi_max7219.o := -DDEBUG

obj-m := spi_max7219.o

build: kernel_modules

kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean
