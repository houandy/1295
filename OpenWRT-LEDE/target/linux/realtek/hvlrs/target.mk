ARCH:=aarch64
SUBTARGET:=hvlrs
BOARDNAME:=I-O DATA NAS
CPU_TYPE:=cortex-a53
CHIP:= rtd129x
KERNELNAME:=Image dtbs

ifeq ("$(CONFIG_RTK_KERNEL_4_4_18)","y")
KERNEL_PATCHVER:=4.4.18
else
KERNEL_PATCHVER:=4.9
endif

define Target/Description
	Build NAS firmware image for Realtek RTD129x SoC boards.
endef
