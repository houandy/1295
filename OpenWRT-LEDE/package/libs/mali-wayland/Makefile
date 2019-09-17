#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mali-wayland
PKG_RELEASE:=2

ifeq ($(CONFIG_TARGET_realtek_rtd16xx),y)
	GPU_ARCH:=bifrost
	PKG_VERSION:=rp16
else ifeq ($(CONFIG_TARGET_realtek_rtd129x),y)
	GPU_ARCH:=midgard
	ifeq ($(CONFIG_RTK_KERNEL_4_4_18),y)
		PKG_VERSION:=rp20
	else
		PKG_VERSION:=rp23
	endif
endif

TOOLCHAIN_VERSION:=OpenWRT-gcc4.9-glibc2.19

PKG_SOURCE_PROTO:=local
PKG_SOURCE:=apps_$(PKG_NAME)_$(GPU_ARCH)_$(PKG_VERSION)_$(TOOLCHAIN_VERSION).tar.xz
PKG_SOURCE_URL:=$(TOPDIR)/../Packages/apps/libs/$(PKG_NAME)/$(PKG_SOURCE)

include $(INCLUDE_DIR)/package.mk

define Package/mali-wayland/Default
  TITLE:=Arm Mali OpenGLES Library
endef

define Package/mali-wayland
$(call Package/mali-wayland/Default)
  SECTION:=xorg-framework
  CATEGORY:=Xorg
  SUBMENU:=Mali
  TITLE:=Mali Libraries with Wayland Support
  DEPENDS:=+libdrm +libwayland +kmod-mali-wayland \
		+!RTK_KERNEL_4_4_18:kmod-mali \
		+RTK_KERNEL_4_4_18:kmod-lib-mali \
		+TARGET_realtek_rtd16xx:libstdcpp
endef

define Build/Prepare
	tar -Jxvf $(PKG_SOURCE_URL) -C $(PKG_BUILD_DIR)/
endef

define Build/Compile
endef

ifeq ($(CONFIG_RTK_KERNEL_4_4_18),y)
define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/lib/pkgconfig/* $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_BUILD_DIR)/include/* $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/lib/* $(1)/usr/lib
#	$(CP) $(PKG_BUILD_DIR)/bin/* $(1)/usr/bin
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
#	$(CP) $(PKG_BUILD_DIR)/bin/* $(1)/usr/bin
endef
else
define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/local/mali-wayland-server/include
	$(INSTALL_DIR) $(1)/usr/local/mali-wayland-server/bin
	$(CP) $(PKG_BUILD_DIR)/usr/* $(1)/usr
endef

define Package/mali-wayland/install
	$(INSTALL_DIR) $(1)/usr
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/local/mali-wayland-server/include
	$(INSTALL_DIR) $(1)/usr/local/mali-wayland-server/bin
	$(CP) $(PKG_BUILD_DIR)/usr/* $(1)/usr
endef
endif

$(eval $(call BuildPackage,mali-wayland))
