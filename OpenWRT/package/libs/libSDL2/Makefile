#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=SDL2
PKG_VERSION:=2.0.4
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:= http://www.libsdl.org/release/
PKG_MD5SUM:=44fc4a023349933e7f5d7a582f7b886e

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

PKG_MAINTAINER:=Felix Fietkau <nbd@openwrt.org>
PKG_LICENSE:=LGPL-2.1+

PKG_BUILD_DEPENDS:=mali-wayland wayland-protocols libwayland libxkbcommon

include $(INCLUDE_DIR)/package.mk

define Package/libSDL2
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=The Simple DirectMedia Layer Version 2
  DEPENDS:= \
	+mali-wayland +wayland-protocols +libwayland +libxkbcommon +alsa-lib
  URL:=http://www.linuxfromscratch.org/blfs/view/cvs/multimedia/sdl2.html
endef

define Package/libSDL2/description
  library provides low level access to audio, keyboard, mouse, joystick, 3D hardware via OpenGL, and 2D frame buffer across multiple platforms.
endef


define Build/InstallDev
	$(CP) $(PKG_INSTALL_DIR)/* $(1)/
endef

define Package/libSDL2/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libSDL2* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libSDL2))
