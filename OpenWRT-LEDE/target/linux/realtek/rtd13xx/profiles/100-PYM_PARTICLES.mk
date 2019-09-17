#
# Copyright (C) 2018 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/pymparticles-emmc
  NAME:=PYM_PARTICLES eMMC
  PACKAGES+=kmod-rtk-emmc
endef

define Profile/pymparticles-emmc/Config
	select RTK_BOARD_CHIP_1319
	select PACKAGE_kmod-rtk-emmc
	select PACKAGE_kmod-fs-squashfs
endef

define Profile/pymparticles-emmc/Description
	PYM_PARTICLES board with eMMC
endef

$(eval $(call Profile,pymparticles-emmc))

define Profile/pymparticles-spi
  NAME:=PYM_PARTICLES SPI
  PACKAGES+=kmod-rtk-spi
endef

define Profile/pymparticles-spi/Config
	select RTK_BOARD_CHIP_1319
	select PACKAGE_kmod-rtk-spi
endef

define Profile/pymparticles-spi/Description
	PYM_PARTICLES board with SPI
endef

$(eval $(call Profile,pymparticles-spi))
