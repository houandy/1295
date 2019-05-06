ARCH:=aarch64
SUBTARGET:=rtd129x
BOARDNAME:=RTD129x NAS
CPU_TYPE:=cortex-a53

KERNELNAME:=Image dtbs

KERNEL_PATCHVER:=4.4.18

define Target/Description
	Build NAS firmware image for Realtek RTD129x SoC boards.
endef
