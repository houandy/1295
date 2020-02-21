ifneq ($(KERNELRELEASE),)
	obj-m := rtk_btusb.o
	rtk_btusb-y = rtk_coex.o rtk_misc.o rtk_bt.o
else
	PWD := $(shell pwd)
	KVER := $(shell uname -r)
	KDIR := /lib/modules/$(KVER)/build
	ARCH := x86

all:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(PWD) modules

clean:
	rm -rf *.o *.mod.c *.mod.o *.ko *.symvers *.order *.a

endif
