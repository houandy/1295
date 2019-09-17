#
# Copyright (C) 2019 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=tinycompress
PKG_VERSION:=1.1.8
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=ftp://ftp.alsa-project.org/pub/tinycompress/
PKG_HASH:=08848b2372eb7addd13d70c72476bbcff8ff0792d2c7fcdda896997e0accb2e9

PKG_LICENSE:=LGPLv2.1 BSD
PKG_LICENSE_FILES:=COPYING aserver/COPYING

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1
PKG_USE_MIPS16:=0
PKG_CHECK_FORMAT_SECURITY:=0

TARGET_SOURCE_URL:=$(TOPDIR)/../Packages/apps/libs/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/tinycompress
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=ALSA tinycompress library
  URL:=http://www.alsa-project.org/
  DEPENDS:=@AUDIO_SUPPORT +kmod-sound-core +libpthread +librt
endef

define Package/tinycompress/description
 This is the library package for ALSA to handle compressed formats like MP3 etc.
 You must have enabled the ALSA support in the kernel.
endef

TARGET_CFLAGS += $(FPIC)

define Build/Prepare
	if [ -d "patches" ]; then rm -f patches/*.patch; else mkdir -p patches; fi
	$(CP) $(TARGET_SOURCE_URL)/*.patch patches/
	$(call Build/Prepare/Default)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/include/tinycompress \
		$(1)/usr/include/

	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libtinycompress.{la,so*} \
		$(1)/usr/lib/
endef

define Package/tinycompress/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libtinycompress.so.* \
		$(1)/usr/lib/
endef

$(eval $(call BuildPackage,tinycompress))
