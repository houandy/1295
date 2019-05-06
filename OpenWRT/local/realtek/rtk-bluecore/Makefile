#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=rtk-bluecore
PKG_RELEASE:=1
RELEASE_DATE:=$(shell date +%Y%m%d)

PKG_SOURCE_PROTO:=local
ifeq ($(SUBTARGET), rtd16xx)
	TARGET_NAME=bluecore.audio.release.SQA_PB.zip
endif
PKG_SOURCE_URL:=$(TOPDIR)/../Packages/fw/bluecore.audio
#PKG_SOURCE_PREPACK_CMD = find . -name .git -o -name system -exec rm -rf {} +
#PKG_GIT_OPT:=--depth 1
PKG_SOURCE_SUBDIR:=$(PKG_NAME)
#PKG_SOURCE_VERSION:=HEAD
PKG_SOURCE:=rtkdl-$(PKG_NAME)-$(SUBTARGET).tar.xz
#DL_XZ_OPTS:=-T0

#ifeq ($(CONFIG_UPDATE_AUDIO_FW),y)
#  PKG_SOURCE_BRANCH:=$(call qstrip,$(CONFIG_AUDIO_FW_TAG))
#endif

PKG_CONFIG_DEPENDS:= \
	CONFIG_TARGET_SUBTARGET \
	CONFIG_AUDIO_FW_TAG

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/rtk-bluecore
  SECTION:=multimedia
  CATEGORY:=Multimedia
  TITLE:=Bluecore Libraries
  DEPENDS:=@(RTK_NAS_TRANSCODE&&(TARGET_realtek_rtd129x||TARGET_realtek_rtd139x||TARGET_realtek_rtd16xx))
  DEFAULT:=y
endef

define Package/rtk-bluecore/description
 Realtek Bluecore libraries
endef

define Package/rtk-bluecore/config
  source "$(SOURCE)/Config.in"
endef

#ifeq ($(SUBTARGET), rtd16xx)
define Build/Prepare
#	$(call Build/Prepare/Default)
#	rm -rf ./files ./bluecore.audio.enc.A00 ; mkdir -p ./files
#	unzip -o $(PKG_SOURCE_URL)/$(TARGET_NAME) bluecore.audio.enc.A00
#	rm -f ./files/bluecore.audio.zip
#	zip -j ./files/bluecore.audio.zip bluecore.audio.enc.A00
endef
#endif
define Build/Compile
endef

define Package/rtk-bluecore/install
	#
endef

$(eval $(call BuildPackage,rtk-bluecore))
