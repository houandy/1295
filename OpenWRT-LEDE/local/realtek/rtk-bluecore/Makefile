#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=rtk-bluecore
PKG_RELEASE:=1

PKG_SOURCE:=bluecore.audio.zip
PKG_SOURCE_PROTO:=local
PKG_SOURCE_URL:=$(TOPDIR)/../Packages/fw/bluecore.audio/$(PKG_SOURCE)

PKG_CONFIG_DEPENDS:= \
	CONFIG_TARGET_SUBTARGET \
	RTK_NAS_TRANSCODE \
	RTK_NAS_VIDEO

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/rtk-bluecore
  SECTION:=firmware
  CATEGORY:=Firmware
  TITLE:=Audio CPU Firmware
  DEPENDS:=@TARGET_realtek
  DEFAULT:=y
endef

define Package/rtk-bluecore/description
  Realtek Audio CPU Firmware
endef

define Package/rtk-bluecore/config
  source "$(SOURCE)/Config.in"
endef

define Build/Prepare
	unzip $(PKG_SOURCE_URL) -d $(PKG_BUILD_DIR)
endef

define Build/Compile
endef

define Package/rtk-bluecore/install
	$(INSTALL_DIR) $(PKG_BUILD_DIR)/release
	$(CP) $(PKG_BUILD_DIR)/$(CONFIG_RTK_ACPU_FW_FILE) $(PKG_BUILD_DIR)/release/bluecore.audio
endef

$(eval $(call BuildPackage,rtk-bluecore))
