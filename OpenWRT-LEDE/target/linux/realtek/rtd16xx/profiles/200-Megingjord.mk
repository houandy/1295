#
# Copyright (C) 2018 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/megingjord-emmc
  NAME:=Megingjord eMMC
  PACKAGES+=kmod-rtk-emmc
endef   

define Profile/megingjord-emmc/Config
	select RTK_BOARD_CHIP_1619
	select PACKAGE_kmod-rtk-emmc
	select PACKAGE_kmod-fs-squashfs
endef

define Profile/megingjord-emmc/Description
	Megingjord board with eMMC
endef   

$(eval $(call Profile,megingjord-emmc))
define Profile/megingjord-spi
  NAME:=Megingjord SPI
  PACKAGES+=kmod-rtk-spi
endef   

define Profile/megingjord-spi/Config
	select RTK_BOARD_CHIP_1619
	select PACKAGE_kmod-rtk-spi
endef

define Profile/megingjord-spi/Description
	Megingjord board with SPI
endef   

$(eval $(call Profile,megingjord-spi))
