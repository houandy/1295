# Copyright (C) 2018 OpenWrt.org
#
include $(TOPDIR)/rules.mk

ARCH:=aarch64
BOARD:=realtek
BOARDNAME:=Realtek SoC
SUBTARGETS:=rtd16xx rtd129x hvlrs
FEATURES:= squashfs usb
MAINTAINER:=william.lee <william.lee@realtek.com>
CFLAGS:=-Os -pipe -fno-caller-saves

KERNEL_PATCHVER:=4.9

include $(INCLUDE_DIR)/target.mk
DEFAULT_PACKAGES += e2fsprogs \


$(eval $(call BuildTarget))
