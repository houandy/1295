#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mali-wayland
PKG_VERSION:=rp20
PKG_RELEASE:=1

TOOLCHAIN_VERSION:=OpenWRT-gcc4.9-glibc2.19

PKG_SOURCE_PROTO:=local
PKG_SOURCE:=apps_$(PKG_NAME)_$(PKG_VERSION)_$(TOOLCHAIN_VERSION).tar.xz
PKG_SOURCE_URL:=$(TOPDIR)/../Packages/apps/libs/$(PKG_NAME)/$(PKG_SOURCE)

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
#KERNEL_BUILD_PATH:=$(LINUX_DIR)

include $(INCLUDE_DIR)/package.mk

define Package/mali-wayland/Default
  TITLE:=Arm Mali OpenGLES Library
endef

define Package/mali-wayland
  SECTION:=xorg-framework
  CATEGORY:=Xorg
  SUBMENU:=Mali
  TITLE:=Mali Libraries with Wayland Support
  DEPENDS:=+libdrm +kmod-lib-mali +libwayland +kmod-mali-wayland
endef

define Build/Prepare
	tar -Jxvf $(PKG_SOURCE_URL) -C $(PKG_BUILD_DIR)/
endef

define Build/Compile
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/lib/pkgconfig/* $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_BUILD_DIR)/include/* $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/lib/* $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/bin/* $(1)/usr/bin
endef


define Package/mali-wayland/install
	$(INSTALL_DIR) $(1)/usr
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/lib/pkgconfig/* $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_BUILD_DIR)/include/* $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/lib/* $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/bin/* $(1)/usr/bin
endef

#define Package/mali-wayland/install
	#$(INSTALL_DIR) $(1)/lib
	#$(CP) $(PKG_BUILD_DIR)/lib/lib* $(1)/lib/
#endef

$(eval $(call BuildPackage,mali-wayland))
