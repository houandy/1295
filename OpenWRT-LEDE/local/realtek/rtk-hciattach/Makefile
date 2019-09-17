#
# Copyright (C) 2015-2016 Realtek
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=rtk-hciattach
PKG_RELEASE:=1
PKG_SOURCE_PROTO:=local
TARGET_SOURCE_URL:=$(TOPDIR)/../Packages/drivers/bluetooth/uart/rtk_hciattach

include $(INCLUDE_DIR)/package.mk

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

define Package/rtk-hciattach
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Realtek BT HCI Attach Tool
  DEPENDS:=+kmod-rtk_btuart
endef

define Package/rtk-hciattach/description
 Realtek BT HCI attach tool
endef

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) $(TARGET_SOURCE_URL)/* $(PKG_BUILD_DIR)
	$(call Build/Prepare/Default)
endef

MAKE_OPTS:= \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	CC="$(TARGET_CC)"

define Build/Compile
	cd $(PKG_BUILD_DIR); \
	$(MAKE) $(MAKE_OPTS)
endef

define Package/rtk-hciattach/install
	$(INSTALL_DIR) $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rtk_hciattach $(1)/usr/bin/
endef

$(eval $(call BuildPackage,rtk-hciattach))
