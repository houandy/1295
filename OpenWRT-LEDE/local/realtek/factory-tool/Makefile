#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=factory-tool
PKG_RELEASE:=1
RELEASE_DATE:=$(shell date +%Y%m%d)

PKG_SOURCE_SUBDIR:=$(PKG_NAME)
TARGET_SOURCE_URL:=$(TOPDIR)/../Packages/apps/utils/$(PKG_NAME)

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)


ifeq ($(CONFIG_DEFAULT_kmod-rtk-emmc),y)
	STORAGE_TYPE:=STOR_TYPE_EMMC
endif
ifeq ($(CONFIG_DEFAULT_kmod-rtk-spi),y)
	STORAGE_TYPE:=STOR_TYPE_SPI
endif
ifeq ($(CONFIG_RTK_BOARD_CHIP_CODENAME), "rtd16xx")
	CHIP_TYPE_NAME:=CHIP_TYPE_THOR
endif
ifeq ($(CONFIG_RTK_BOARD_CHIP_CODENAME), "rtd12xx")
	CHIP_TYPE_NAME:=CHIP_TYPE_KYLIN
endif
ifeq ($(CONFIG_RTK_BOARD_CHIP_CODENAME), "rtd13xx")
	CHIP_TYPE_NAME:=CHIP_TYPE_HANK
endif


INCLUDES:= -I./include -I./ -I./utils
DEFINES:= -DNAS_ENABLE -DSYNO -D$(STORAGE_TYPE) -D$(CHIP_TYPE_NAME)
ADD_CFLAGS:= -march=armv8-a -Wall
LIBS:= libfactory.a

ifeq ($(lastword $(TARGET_AR)), cr)
	TARGET_AR:=$(TARGET_AR)
else
	TARGET_AR:=$(TARGET_AR) cr
endif

include $(INCLUDE_DIR)/package.mk

define Package/factory-tool
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Realtek Factory Tool
	DEFAULT:=y
endef

define Package/factory-tool/description
	Realtek Factory Tool.
endef

define Build/Compile
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		CC="$(TARGET_CROSS)gcc" \
		AR="$(TARGET_AR)" \
		RANLIB="$(TARGET_RANLIB)" \
		CFLAGS="$(TARGET_CFLAGS) $(INCLUDES) $(DEFINES) $(ADD_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		LIBS="$(LIBS)"
endef

define Build/Prepare
	$(CP) $(TARGET_SOURCE_URL)/* $(PKG_BUILD_DIR)/
endef


define Package/factory-tool/install
	$(INSTALL_DIR) $(1)/root/utils
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/factory $(1)/root/utils
endef

$(eval $(call BuildPackage,factory-tool))
