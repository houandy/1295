#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=rtk-imagefile
PKG_RELEASE:=1
RELEASE_DATE:=$(shell date +%Y%m%d)

PKG_SOURCE_PROTO:=local
PKG_SOURCE_URL:=$(TOPDIR)/../Image-Builder
PKG_SOURCE_SUBDIR:=$(PKG_NAME)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_SOURCE_VERSION).tar.xz
DL_XZ_OPTS:=-T0

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

IMG_DST_DIR:=$(TOPDIR)/target/linux/$(BOARD)/image/$(PKG_NAME)
IMG_OMV_DIR:=$(IMG_DST_DIR)/components/packages/omv
IMG_EXTRA_DIRS:= data cache

include $(INCLUDE_DIR)/package.mk

define Package/rtk-imagefile
  TITLE:=Image file creator for Realtek platform
  DEPENDS:=@TARGET_realtek
endef

define Package/rtk-imagefile/description
 Realtek image file creator for Realtek FW layout partition with fwdesc table.
endef

define Package/rtk-imagefile/config
        source "$(SOURCE)/Config.in"
endef

define Build/Configure
endef
#ifeq ($(SUBTARGET), rtd16xx)
define Build/Prepare
#	$(call Build/Prepare/Default)
	mkdir -p $(IMG_DST_DIR)
	$(CP) $(PKG_SOURCE_URL)/* $(IMG_DST_DIR)/
#	$(SED) "s/^\(IMGFILE_VER=\).*/\1g$(PKG_SOURCE_VERSION)/" $(PKG_BUILD_DIR)/Makefile.in
#	rm -rf $(IMG_DST_DIR)
#	mkdir -p $(IMG_DST_DIR)
#	mv $(PKG_BUILD_DIR)/* $(IMG_DST_DIR)/
#	cd $(IMG_OMV_DIR) && mkdir -p $(IMG_EXTRA_DIRS)
endef
#endif

define Build/Compile
endef

define Package/rtk-imagefile/install
	#
endef

$(eval $(call BuildPackage,rtk-imagefile))
