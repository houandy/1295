ARCH:=aarch64
SUBTARGET:=rtd13xx
BOARDNAME:=RTD13xx NAS
CPU_TYPE:=cortex-a55

KERNELNAME:=Image dtbs

KERNEL_PATCHVER:=4.9

define Target/Description
	Build NAS firmware image for Realtek RTD13xx SoC boards.
endef
