#
# Copyright (C) 2015-2017 Realtek
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=rtk-init
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

TARGET_CFLAGS += -D_GNU_SOURCE

define Package/rtk-init
  SECTION:=base
  CATEGORY:=Base system
  TITLE:=init process for Realtek SoC
  DEPENDS:=@TARGET_realtek @RTK_BOARD_FWDESC_LAYOUT
  DEPENDS+=+libc +ip-full
  DEPENDS+=+@KERNEL_NAMESPACES +@KERNEL_PID_NS +@KERNEL_NET_NS
endef

define Package/rtk-init/description
 Realtek SoC init process for OpenWRT platform
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/rtk-init/install
	$(INSTALL_DIR) $(1)/etc/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/init $(1)/etc
endef

$(eval $(call BuildPackage,rtk-init))
