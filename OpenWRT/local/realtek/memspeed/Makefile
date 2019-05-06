#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=memspeed
PKG_RELEASE:=1
RELEASE_DATE:=$(shell date +%Y%m%d)

#PKG_SOURCE_PROTO:=gits
#PKG_SOURCE_PREPACK_CMD = find . -name .git -o -name system -exec rm -rf {} +
#PKG_GIT_OPT:=--depth 1
PKG_SOURCE_SUBDIR:=$(PKG_NAME)
#PKG_SOURCE_BRANCH:=master
#PKG_SOURCE_VERSION:=HEAD
#PKG_SOURCE:=rtkdl-$(PKG_NAME)-$(SUBTARGET).tar.xz
#DL_XZ_OPTS:=-T0


PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/memspeed
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Memory Speed Test
  DEFAULT:=n
endef

define Package/memspeed/description
 Memory Speed Test Program
endef

define Build/Compile
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		CC="$(TARGET_CC_NOCACHE)" 
endef

define Build/Prepare
	$(CP) files/* $(PKG_BUILD_DIR)/
endef


define Package/memspeed/install
	$(INSTALL_DIR) $(1)/root/utils
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/memspeed_a53 $(1)/root/utils/
endef

$(eval $(call BuildPackage,memspeed))
