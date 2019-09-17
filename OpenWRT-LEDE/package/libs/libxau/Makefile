#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# blogic@openwrt.org
include $(TOPDIR)/rules.mk

PKG_NAME:=libxau
PKG_DL_NAME:=libXau
PKG_RELEASE:=1
PKG_VERSION:=X11R7.2-1.0.3
PKG_SOURCE_URL:=http://xorg.freedesktop.org/releases/X11R7.2/src/lib/
PKG_SOURCE:=$(PKG_DL_NAME)-$(PKG_VERSION).tar.bz2
PKG_BUILD_DIR=$(BUILD_DIR)/Xorg/${PKG_DL_NAME}-$(PKG_VERSION)/
PKG_MD5SUM:=4731e3699574eb868195ea6f6d3a12cf
PKG_BUILD_DEPENDS:=xproto

include $(INCLUDE_DIR)/package.mk

define Package/libxau
  SECTION:=xorg-framework
  CATEGORY:=Xorg
  SUBMENU:=libraries
  #DEPENDS:=+xorg-headers-native +util-macros +xproto
  TITLE:=Xau Libraries
  URL:=http://xorg.freedesktop.org/
endef

define Build/InstallDev
	DESTDIR=$(1) $(MAKE) -C $(PKG_BUILD_DIR)/ $(MAKE_FLAGS) install
endef

ifeq (libxau,libX11)
 CONFIGURE_ARGS_XTRA=--without-xcb
endif

define libX11/Compile
	$(MAKE_VARS) \
		$(MAKE) -C $(PKG_BUILD_DIR)/src/util CFLAGS="" LDFLAGS="" CC="cc" makekeys
endef

define libXt/Compile
	$(MAKE_VARS) \
		$(MAKE) -C $(PKG_BUILD_DIR)/util CFLAGS="" LDFLAGS="" CC="cc"
endef

define Build/Compile
	$(call $(PKG_NAME)/Compile)
	make -C $(PKG_BUILD_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	DESTDIR=$(PKG_INSTALL_DIR) $(MAKE) -C $(PKG_BUILD_DIR) $(MAKE_FLAGS) install
	find $(PKG_INSTALL_DIR) -name *.la | xargs rm -rf
endef

define Build/Configure
	(cd $(PKG_BUILD_DIR)/$(CONFIGURE_PATH); \
	if [ -x $(CONFIGURE_CMD) ]; then \
		$(CP) $(SCRIPT_DIR)/config.{guess,sub} $(PKG_BUILD_DIR)/ && \
		$(CONFIGURE_VARS) \
		$(CONFIGURE_CMD) \
		$(CONFIGURE_ARGS_XTRA) \
		$(CONFIGURE_ARGS) \
		--enable-malloc0returnsnull; \
	fi \
	)
endef

define Package/libxau/install/Default
	$(INSTALL_DIR) $(1)/usr/lib
	find $(PKG_INSTALL_DIR)/usr/lib/ -name lib*so* | $(XARGS) -I % -t cp -P % $(1)/usr/lib
endef

define Package/libxau/install
	$(call Package/libxau/install/Default,$(1))
endef

define Package/libXaw/install
	$(call Package/libxau/install/Default,$(1))
	cd $(1)/usr/lib; rm -f libXaw.so.7 libXaw.so.6; ln -s libXaw7.so.7.0.0 libXaw.so.7; ln -s libXaw6.so.6.0.1 libXaw.so.6
endef

define Build/InstallDev
	$(CP) $(PKG_INSTALL_DIR)/* $(1)
endef

$(eval $(call BuildPackage,libxau))
