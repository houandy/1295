#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=openmax
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

TOOLCHAIN:=OpenWRT-gcc8.2-glibc2.27
PKG_SOURCE_PROTO:=local

ifdef CONFIG_RTK_BOARD_CHIP_1619
ifdef CONFIG_RTK_NAS_TRANSCODE
TARGET_SOURCE_URL:=$(TOPDIR)/../Packages/apps/mm/ffmpeg
TARGET_SOURCE:=apps_ffmpeg_3.3.7_OpenWRT-gcc8.2-glibc2.27.tar.xz
ifdef CONFIG_RTK_NAS_VIDEO
TARGET_SOURCE_URL:=$(TOPDIR)/../Packages/apps/mm/player
TARGET_SOURCE:=apps_player_3.3.7_OpenWRT-gcc8.2-glibc2.27.tar.xz
endif
endif
else
ifdef CONFIG_RTK_NAS_TRANSCODE
TARGET_SOURCE_URL:=$(TOPDIR)/../Packages/apps/mm/ffmpeg
TARGET_SOURCE:=apps_ffmpeg_4.0.3_OpenWRT-gcc8.2-glibc2.27.tar.xz
ifdef CONFIG_RTK_NAS_VIDEO
TARGET_SOURCE_URL:=$(TOPDIR)/../Packages/apps/mm/player
TARGET_SOURCE:=apps_player_4.0.3_OpenWRT-gcc8.2-glibc2.27.tar.xz
endif
endif
endif
TARGET_NAME:=RTK-NAS-Transcode-Libs
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk
define Package/openmax
  SECTION:=multimedia
  CATEGORY:=Multimedia
#  SUBMENU:=Multimedia
  TITLE:=Realtek OpenMAX libraries
  DEPENDS:=@RTK_NAS_TRANSCODE +libstdcpp $(LIBRPC_DEPENDS) +alsa-lib 
endef

define Package/openmax/description
 Realtek OpenMAX libraries and various daemons
endef

define Build/Prepare
endef

define Build/Compile
endef

define Build/InstallDev
	tar -C $(1) --strip-components=1 -Jxf $(TARGET_SOURCE_URL)/$(TARGET_SOURCE) $(TARGET_NAME)/usr
endef

define Package/openmax/install
	tar -C $(1) --strip-components=1 -Jxf $(TARGET_SOURCE_URL)/$(TARGET_SOURCE) $(TARGET_NAME)/usr
  ifdef CONFIG_RTK_BOARD_CHIP_1295
	tar -C $(1) --strip-components=1 -Jxf $(TARGET_SOURCE_URL)/$(TARGET_SOURCE) $(TARGET_NAME)/system
  endif
  ifdef CONFIG_RTK_BOARD_CHIP_1296
	tar -C $(1) --strip-components=1 -Jxf $(TARGET_SOURCE_URL)/$(TARGET_SOURCE) $(TARGET_NAME)/system
  endif
  ifeq ("$(CONFIG_RTK_KERNEL_4_4_18)","y")
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/rtk-supplicant.init $(1)/etc/init.d/rtk-supplicant
  endif
endef

$(eval $(call BuildPackage,openmax))
