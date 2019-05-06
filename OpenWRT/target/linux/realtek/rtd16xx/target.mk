ARCH:=aarch64
SUBTARGET:=rtd16xx
BOARDNAME:=RTD16xx NAS
CPU_TYPE:=cortex-a55

KERNELNAME:=Image dtbs

ifeq ("$(CONFIG_RTK_KERNEL_4_4_162)","y")
KERNEL_PATCHVER:=4.4
else
ifeq ("$(CONFIG_RTK_KERNEL_4_9)","y")
KERNEL_PATCHVER:=4.9
else
KERNEL_PATCHVER:=4.9
endif
endif

define Target/Description
	Build NAS firmware image for Realtek RTD16xx SoC boards.
endef
