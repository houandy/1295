#
# Copyright (C) 2011-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=fan
PKG_RELEASE:=2

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/fan
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=This app can control NCT3941SA1 fan, and keep a box cooling.
  DEPENDS:=+libpthread
endef

define Package/fan/description
 This app can control NCT3941SA1 fan, and keep a box cooling.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		LINUX="$(LINUX_DIR)" \
		CC="$(TARGET_CC)" \
		STAGING_DIR="$(STAGING_DIR)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		CFLAGS="$(TARGET_CFLAGS)"
endef

define Package/fan/install
	$(INSTALL_DIR) $(1)/etc/init.d $(1)/usr/sbin/ $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/fan $(1)/usr/sbin/
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,fan))
