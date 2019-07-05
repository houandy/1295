ARCH:=aarch64
SUBTARGET:=hvlrs
BOARDNAME:=I-O DATA NAS
CPU_TYPE:=cortex-a53
CHIP:= rtd129x
KERNELNAME:=Image dtbs

KERNEL_PATCHVER:=4.4.18

define Target/Description
	Build NAS firmware image for Realtek RTD129x SoC boards.
endef
