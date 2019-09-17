#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libwayland
PKG_VERSION:=1.12.0
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_URL:=http://wayland.freedesktop.org/releases/
PKG_SOURCE:=wayland-$(PKG_VERSION).tar.xz
PKG_MD5SUM:=ee615e2d1e205eec48b02f069327eb96
PKG_FIXUP:=libtool
PKG_BUILD_DEPENDS:=\
	libxml2/host \
	libffi/host \
	libwayland/host
HOST_BUILD_DEPENDS:=libxml2/host

PKG_INSTALL:=1
HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/wayland-$(PKG_VERSION)
PKG_BUILD_DIR:=$(BUILD_DIR)/wayland-$(PKG_VERSION)
include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/package.mk

define Package/libwayland
  SECTION:=xorg-framework
  CATEGORY:=Xorg
  SUBMENU:=Wayland
  TITLE:=Wayland Libraries
  DEPENDS:=+libffi +libexpat +libpthread +librt +libxml2
  URL:=http://cgit.freedesktop.org/wayland/wayland
endef

define Package/libwayland/description
  Wayland is intended as a simpler replacement for X, easier to develop and maintain.
  This package contains wayland libraries.
endef

CONFIGURE_ARGS+= --disable-documentation
CONFIGURE_ARGS+= --with-host-scanner
HOST_CONFIGURE_ARGS+=--disable-documentation
#HOST_CONFIGURE_CMD = $(BASH) ./autogen.sh

#define Host/Prepare
#       $(HOST_UNPACK)
#endef

#define Host/Configure
#	( cd "$(HOST_BUILD_DIR)"; ./configure $(HOST_CONFIGURE_ARGS) --disable-documentation)
#endef

#define Build/Configure
#	( cd "$(PKG_BUILD_DIR)"; ./autogen.sh $(TARGET_CONFIGURE_ARGS) --disable-documentation)
#	$(call Build/Configure/Default)
#endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/*.h $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/share/aclocal
	$(CP) $(PKG_INSTALL_DIR)/usr/share/aclocal/wayland-scanner.m4 $(1)/usr/share/aclocal
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libwayland*.so* $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/wayland-client.pc $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/wayland-cursor.pc $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/wayland-server.pc $(1)/usr/lib/pkgconfig
	$(CP) ./files/wayland-scanner.pc $(1)/usr/lib/pkgconfig
	$(INSTALL_DIR) $(1)/usr/share/wayland
	$(CP) $(PKG_INSTALL_DIR)/usr/share/wayland/*.* $(1)/usr/share/wayland
endef

define Package/libwayland/install
	$(INSTALL_DIR) $(1)/usr/lib \
		$(1)/usr/bin \
		$(1)/usr/share/wayland
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libwayland*.so* $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/wayland-scanner $(1)/usr/bin
endef

$(eval $(call HostBuild))
$(eval $(call BuildPackage,libwayland))
