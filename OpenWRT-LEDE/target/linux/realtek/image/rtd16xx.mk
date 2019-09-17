define Device/rtd16xx
  #DEVICE_DTS := rtd-1619-mjolnir-2GB
  #DEVICE_DTS_DIR := $(DTS_DIR)/realtek/rtd16xx
  IMAGES := sysupgrade.bin

ifneq ($(CONFIG_PACKAGE_rtk-bluecore),)
  AFW_CMD = $(STAGING_DIR_HOST)/bin/unzip -o $(TOPDIR)/feeds/realtek/rtk-bluecore/files/$$(AUDIO_FW_PATH)/$$(AUDIO_FW_NAME)
else
  AFW_CMD = $(STAGING_DIR_HOST)/bin/unzip -o $(TOPDIR)/bluecore.audio.thor.zip
endif
endef

define Device/rtd16xx/spi
  $(call Device/rtd16xx)
  DEVICE_PACKAGES := kmod-rtk-spi
  SUPPORTED_DEVICES = $$(BOARD_NAME)-spi

  MTDPARTS := RtkSFC:1280k(Boot)ro,$$(LOGO_SIZE)(logo),$$(AFW_SIZE)(afw),$$(DTB_SIZE)(dtb),$$(KERNEL_SIZE)(kernel),$$(ROOTFS_SIZE)(initrd)
  CMDLINE += rdinit=/sbin/init
  #CMDLINE += INITRAMFS=1
  DTB_CMD = $(SED) 's|\(bootargs\)\s*=\s*"|\1="$$(CMDLINE) |g' $$(DTS_FILE)

  IMAGE/default = $$(IMAGE/logo) | $$(IMAGE/afw) | $$(IMAGE/kernel) | $$(IMAGE/rootfs)

  IMAGE_FW_SIZE = $$(LOGO_SIZE)+$$(AFW_SIZE)+$$(DTB_SIZE)+$$(KERNEL_SIZE)+$$(ROOTFS_SIZE)+32+1

  IMAGE/full.bin = $$(IMAGE/full) | $$(IMAGE/pack) $$(IMAGE_FULL_SIZE)
  IMAGE/mp.bin = $$(IMAGE/mp) | $$(IMAGE/pack) $$(IMAGE_FULL_SIZE)+131072
  IMAGE/sysupgrade.bin = $$(IMAGE/default) | $$(IMAGE/pack) $$(IMAGE_FW_SIZE)
  IMAGE/rootfs.bin = $$(IMAGE/rootfs) | $$(IMAGE/pack) $$(ROOTFS_SIZE)+32+1
  IMAGE/kernel.bin = $$(IMAGE/kernel) | $$(IMAGE/pack) $$(DTB_SIZE)+$$(KERNEL_SIZE)+32+1
  IMAGE/dtb.bin = $$(IMAGE/dtb) | $$(IMAGE/pack) $$(DTB_SIZE)+32+1
  IMAGE/afw.bin = $$(IMAGE/afw) | $$(IMAGE/pack) $$(AFW_SIZE)+32+1
  IMAGE/logo.bin = $$(IMAGE/logo) | $$(IMAGE/pack) $$(LOGO_SIZE)+32+1
endef

# 8MB: 7552kb + 32 byte
#IMAGE_FW_SIZE := 7733280
define Device/generic-spi_8M
  $(call Device/rtd16xx/spi)
  BOARD_NAME := generic

  IMAGES := rootfs.bin afw.bin logo.bin
endef

#TARGET_DEVICES += generic-spi_8M

define Device/mjolnir-spi_8M/Config
	select RTK_BOARD_CHIP_1619
	select PACKAGE_kmod-rtk-spi
endef

define Device/mjolnir-spi_8M
  $(call Device/rtd16xx/spi)
  BOARD_NAME :=mjolnir
  DEVICE_TITLE = $$(BOARD_NAME) with 8MB SPI

  IMAGES += kernel.bin dtb.bin
  IMAGE/dtb = append-dtb $$(DTB_SIZE) $$(CONFIG_RTK_BOARD_CHIP)-mjolnir
endef

#TARGET_DEVICES += mjolnir-spi_8M


LAN_ETH:=eth0

ifeq ($(CONFIG_PACKAGE_kmod-cfg80211),y)
USER_NAME:=$(shell id -u -n)
define Image/Prepare/Setconfig
	sed -i -e '/\b$(LAN_ETH)\b/,/static/ s/static/dhcp/g' $(TARGET_DIR)/etc/config/network
	sed -i '/192.168/d' $(TARGET_DIR)/etc/config/network
	sed -i "/\b$(LAN_ETH)\b/a\        option type 'bridge'" $(TARGET_DIR)/etc/config/network
endef
else
define Image/Prepare/Setconfig
	rm -f $(TARGET_DIR)/etc/config/wireless
endef
endif
