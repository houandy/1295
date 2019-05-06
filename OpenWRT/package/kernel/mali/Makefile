#
# Copyright (C) 2014-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mali
PKG_RELEASE=1

PKG_SOURCE:=drivers_mali_src_biforst_r16p0.tar.xz
PKG_SOURCE_URL:=$(TOPDIR)/../Packages/drivers/mali/$(PKG_SOURCE)
PKG_SOURCE_PROTO:=local
PKG_VERSION:=r16p0

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/mali
  SUBMENU:=GPU Drivers
  TITLE:=ARM Mali GPU driver
  DEPENDS:=@TARGET_realtek&&TARGET_realtek_rtd16xx
  FILES:=$(PKG_BUILD_DIR)/driver/product/kernel/drivers/gpu/arm/midgard/mali_kbase.ko \
		 $(PKG_BUILD_DIR)/driver/product/kernel/drivers/base/dma_buf_test_exporter/dma-buf-test-exporter.ko \
		 $(PKG_BUILD_DIR)/driver/product/kernel/drivers/gpu/arm/midgard/tests/kutf/kutf.ko
  AUTOLOAD:=$(call AutoLoad,50,mali_kbase dma-buf-test-exporter.ko kutf.ko)
endef

define KernelPackage/mali/realtek/rtd129x
  BACON:=RTD129x
endef

define KernelPackage/mali/realtek/rtd16xx
  BACON:=rtd16xx
endef

define Build/Prepare
	tar -Jxvf $(PKG_SOURCE_URL) -C $(PKG_BUILD_DIR)
	$(SED) '/make.include/d' $(PKG_BUILD_DIR)/Makefile
	$(Build/Patch)
endef

MAKE_FLAGS = \
	ARCH=arm64 \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	LINUX_KERNEL_PATH="../linux-$(LINUX_VERSION)"


$(eval $(call KernelPackage,mali))
