#
# Copyright (C) 2018 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/mjolnir-emmc
  NAME:=Mjolnir eMMC
  PACKAGES+=kmod-rtk-emmc
endef   

define Profile/mjolnir-emmc/Config
	select RTK_BOARD_CHIP_1619
	select PACKAGE_kmod-rtk-emmc
	select PACKAGE_kmod-fs-squashfs
endef

define Profile/mjolnir-emmc/Description
	Mjolnir board with eMMC
endef   

$(eval $(call Profile,mjolnir-emmc))

define Profile/mjolnir-spi
  NAME:=Mjolnir SPI
  PACKAGES+=kmod-rtk-spi
endef   

define Profile/mjolnir-spi/Config
	select RTK_BOARD_CHIP_1619
	select PACKAGE_kmod-rtk-spi
endef

define Profile/mjolnir-spi/Description
	Mjolnir board with SPI
endef   

$(eval $(call Profile,mjolnir-spi))
