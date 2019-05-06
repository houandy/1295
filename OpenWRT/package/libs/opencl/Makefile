#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=opencl
ifeq ($(SUBTARGET), rtd16xx)
PKG_VERSION:=r16p0
endif
PKG_RELEASE:=2

TOOLCHAIN_VERSION:=linaro-4.9-2016.02-x86_64_aarch64-linux-gnu

PKG_SOURCE_PROTO:=local
PKG_SOURCE:=apps_$(PKG_NAME)_$(PKG_VERSION)_$(TOOLCHAIN_VERSION).tar.xz
PKG_SOURCE_URL:=$(TOPDIR)/../Packages/apps/libs/$(PKG_NAME)/$(PKG_SOURCE)

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/opencl/Default
  TITLE:=Arm Mali OpenCL Library
endef

define Package/opencl
$(call Package/opencl/Default)
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE+= (library)
  DEPENDS:=+libstdcpp +kmod-mali
endef

define Build/Prepare
	tar -Jxvf $(PKG_SOURCE_URL) -C $(PKG_BUILD_DIR)/
endef

define Build/Compile
endef

define Package/opencl/install
	$(INSTALL_DIR) $(1)/lib
	$(CP) $(PKG_BUILD_DIR)/lib/lib* $(1)/lib/
endef

$(eval $(call BuildPackage,opencl))
