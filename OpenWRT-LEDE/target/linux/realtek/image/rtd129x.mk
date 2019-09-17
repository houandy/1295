define Device/rtd129x
  #DEVICE_DTS := rtd-1296-saola-2GB
  #DEVICE_DTS_DIR := $(DTS_DIR)/realtek/rtd129x
  IMAGES := sysupgrade.bin

ifneq ($(CONFIG_PACKAGE_rtk-bluecore),)
  AFW_CMD = $(STAGING_DIR_HOST)/bin/unzip -o $(TOPDIR)/feeds/realtek/rtk-bluecore/files/$$(AUDIO_FW_PATH)/$$(AUDIO_FW_NAME)
endif
endef

define Device/rtd129x/spi
  $(call Device/rtd129x)
  DEVICE_PACKAGES := kmod-rtk-spi
  SUPPORTED_DEVICES = $$(BOARD_NAME)-spi

  MTDPARTS := RtkSFC:128k(factory),$$(UBOOT_SIZE)(uboot),$$(LOGO_SIZE)(logo),$$(AFW_SIZE)(afw),$$(DTB_SIZE)(dtb),$$(KERNEL_SIZE)(kernel),$$(ROOTFS_SIZE)(initrd)
  CMDLINE += rdinit=/sbin/init
  #CMDLINE += INITRAMFS=1
  DTB_CMD = $(SED) 's|\(bootargs\)\s*=\s*"|\1="$$(CMDLINE) |g' $$(DTS_FILE)

ifeq ($(CONFIG_PACKAGE_u-boot64),y)
UBOOT_FILE := $(BIN_DIR)/u-boot64/openwrt-$(CONFIG_UBoot_BOARD)-bootcode.bin
endif

  IMAGE/default = $$(IMAGE/logo) | $$(IMAGE/afw) | $$(IMAGE/kernel) | $$(IMAGE/rootfs)
  IMAGE/mp = append-env $$(BLOCKSIZE) | append-factory $$(BLOCKSIZE) | $$(IMAGE/uboot) | $$$$(IMAGE/default)

  IMAGE_FW_SIZE = $$(LOGO_SIZE)+$$(AFW_SIZE)+$$(DTB_SIZE)+$$(KERNEL_SIZE)+$$(ROOTFS_SIZE)+32+1
  IMAGE_FULL_SIZE = $$(IMAGE_FW_SIZE)+$$(UBOOT_SIZE)

  IMAGE/uboot.bin = $$(IMAGE/uboot) | $$(IMAGE/pack) $$(UBOOT_SIZE)+32+1
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
  $(call Device/rtd129x/spi)
  BOARD_NAME := generic

  IMAGES := rootfs.bin afw.bin logo.bin
endef

#TARGET_DEVICES += generic-spi_8M

define Device/giraffe-spi_8M/Config
	select RTK_BOARD_CHIP_1295
	select PACKAGE_kmod-rtk-spi
endef

define Device/giraffe-spi_8M
  $(call Device/rtd129x/spi)
  BOARD_NAME := giraffe
  DEVICE_TITLE = $$(BOARD_NAME) with 8MB SPI

  IMAGES += kernel.bin dtb.bin
ifeq ($(CONFIG_PACKAGE_u-boot64)$(CONFIG_RTK_BOARD_CHIP_1295),yy)
    IMAGES += uboot.bin
    IMAGES += full.bin mp.bin
endif
  IMAGE/dtb = append-dtb $$(DTB_SIZE) rtd-1295-giraffe
endef

#TARGET_DEVICES += giraffe-spi_8M

define Device/saola-spi_8M/Config
	select RTK_BOARD_CHIP_1296
	select PACKAGE_kmod-rtk-spi
endef

define Device/saola-spi_8M
  $(call Device/rtd129x/spi)
  BOARD_NAME :=saola
  DEVICE_TITLE = $$(BOARD_NAME) with 8MB SPI

  IMAGES += kernel.bin dtb.bin
ifeq ($(CONFIG_PACKAGE_u-boot64)$(CONFIG_RTK_BOARD_CHIP_1296),yy)
    IMAGES += uboot.bin
    IMAGES += full.bin mp.bin
endif
  IMAGE/dtb = append-dtb $$(DTB_SIZE) rtd-1296-saola
endef

#TARGET_DEVICES += saola-spi_8M

ifeq ($(CONFIG_RTD_1295_HWNAT),y)
LAN_ETH:=eth1
else
LAN_ETH:=eth0
endif

ifeq ($(CONFIG_PACKAGE_kmod-cfg80211),y)
USER_NAME:=$(shell id -u -n)
define Image/Prepare/Setconfig
	sed -i 's/OpenWrt_2.4G$$$$/OpenWrt_2.4G_$(USER_NAME)/g' $(TARGET_DIR)/etc/config/wireless
	sed -i 's/OpenWrt_5G$$$$/OpenWrt_5G_$(USER_NAME)/g' $(TARGET_DIR)/etc/config/wireless
	sed -i 's/GuestWRT_5G$$$$/GuestWRT_5G_$(USER_NAME)/g' $(TARGET_DIR)/etc/config/wireless
	sed -i "/\b$(LAN_ETH)\b/a\      option type 'bridge'" $(TARGET_DIR)/etc/config/network
	if [ "$(CONFIG_RTD_1295_HWNAT)" = "y" ]; then \
		sed -i 's|192.168.0.9|192.168.1.9|g' $(TARGET_DIR)/etc/config/network; \
		cat ../conf/network.nat >> $(TARGET_DIR)/etc/config/network; \
	else \
		sed -i 's|wan|lan|g' $(TARGET_DIR)/etc/config/network; \
	fi
	cat ../conf/network.guest >> $(TARGET_DIR)/etc/config/network
	$(CP) $(CURDIR)/../rtd129x/files/factory $(TARGET_DIR)/usr/local/bin
endef
else
define Image/Prepare/Setconfig
	rm -f $(TARGET_DIR)/etc/config/wireless
	rm -f $(TARGET_DIR)/etc/resolv.conf
	$(CP) ../conf/resolv.conf $(TARGET_DIR)/etc/resolv.conf
endef
endif
