# 
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

OTHER_MENU:=Other modules

PKG_NAME:=rtk_btusb
PKG_VERSION:=1
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=local
TARGET_SOURCE_URL:=$(TOPDIR)/../Packages/drivers/bluetooth/usb/bluetooth_usb_driver

PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_SOURCE_SUBDIR)

include $(INCLUDE_DIR)/package.mk

define KernelPackage/rtk_btusb
  SUBMENU:=$(OTHER_MENU)
  DEPENDS:=+kmod-bluetooth @!PACKAGE_kmod-bluetooth_usb_driver
  TITLE:=Realtek bluetooth USB driver for Linux
  FILES:=$(PKG_BUILD_DIR)/rtk_btusb.ko
  AUTOLOAD:=$(call AutoProbe,rtk_btusb)
endef

define KernelPackage/rtk_btusb/description
Realtek bluetooth USB driver for Linux
endef

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) $(TARGET_SOURCE_URL)/* $(PKG_BUILD_DIR)/
	#$(call Build/Prepare/Default)
endef

MAKE_OPTS:= CROSS_COMPILE="$(TARGET_CROSS)" \
	KDIR="${LINUX_DIR}" \
	ARCH="${LINUX_KARCH}"

define Build/Compile
	$(CP) $(LINUX_DIR)/Module.symvers $(PKG_BUILD_DIR)/; \
	cd $(PKG_BUILD_DIR); \
	$(MAKE) $(MAKE_OPTS);
endef

$(eval $(call KernelPackage,rtk_btusb))
