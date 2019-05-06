#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ufsd
ifeq ($(SUBTARGET), rtd16xx)
  ifeq ($(KERNEL_PATCHVER), 4.4)
    PKG_VERSION:=9.4.4
    FS_LIB:=lke_$(PKG_VERSION)_b4
  else
    PKG_VERSION:=9.6.4
	FS_LIB:=lke_$(PKG_VERSION)_b14
  endif
else ifeq ($(SUBTARGET), rtd129x)
  ifeq ($(KERNEL_PATCHVER), 4.4.18)
    PKG_VERSION:=9.4.4
	FS_LIB:=lke_$(PKG_VERSION)_b4
  endif
endif
UTIL_VER:=$(FS_LIB)_utilites

PKG_RELEASE:=1

TOOLCHAIN_VERSION:=OpenWRT-gcc8.2-glibc2.27
#TOOLCHAIN_VERSION:=ARM-GCC8.2-glibc2.28

PKG_SOURCE_PROTO:=local
PKG_SOURCE:=apps_$(PKG_NAME)_kernel-$(KERNEL_PATCHVER)_$(FS_LIB)_$(TOOLCHAIN_VERSION).txz
PKG_SOURCE_URL:=$(TOPDIR)/../Packages/apps/utils/$(PKG_NAME)/$(PKG_SOURCE)

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/ufsd-$(PKG_VERSION)

#KERNEL_BUILD_PATH:=$(LINUX_DIR)

include $(INCLUDE_DIR)/package.mk

define KernelPackage/ufsd
  VERSION:=$(LINUX_VERSION)-$(BOARD)-$(PKG_RELEASE)
  SUBMENU:=Filesystems
  TITLE:=Paragon's File System
  FILES:= \
	$(PKG_BUILD_DIR)/ufsd.ko \
	$(PKG_BUILD_DIR)/jnl.ko
  AUTOLOAD:=$(call AutoProbe,ufsd)
  DEPENDS:=+libc
endef

define KernelPackage/ufsd/Default/description
  Parago's driver for NTFS/exFAT/HFS+
endef

define Build/Prepare
	tar -Jxvf $(PKG_SOURCE_URL) -C $(PKG_BUILD_DIR)
endef

define Build/Compile
endef

define KernelPackage/ufsd/install
	$(INSTALL_DIR) $(1)/usr/local/sbin
	$(CP) $(PKG_BUILD_DIR)/$(UTIL_VER)/* $(1)/usr/local/sbin
endef

$(eval $(call KernelPackage,ufsd))
