#
# Copyright (C) 2007-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=net-service
PKG_VERSION:=1
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/net-service
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Realtek Network Config Service
  DEPENDS:=
endef

define Package/net-service/description
	Realtek Network Config Service
endef

define Build/Compile
endef

define Package/net-service/install
	$(INSTALL_DIR) $(1)/usr/local/sbin
	$(INSTALL_BIN) ./files/net-service $(1)/usr/local/sbin/
	$(INSTALL_BIN) ./files/net-status $(1)/usr/local/sbin/
	$(INSTALL_DIR) $(1)/usr/local/etc/
	$(CP) -r ./files/network_config $(1)/usr/local/etc/
endef

$(eval $(call BuildPackage,net-service))
