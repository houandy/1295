# 
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=rtk-bt-firmware
PKG_VERSION:=1
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)


UART_FW_DIR:=$(TOPDIR)/../Packages/drivers/bluetooth/uart/firmware
USB_FW_DIR:=$(TOPDIR)/../Packages/drivers/bluetooth/usb/firmware

include $(INCLUDE_DIR)/package.mk

define Package/rtk-btfw-default
  SECTION:=firmware
  CATEGORY:=Firmware
  TITLE:=$(1)
  DEPENDS:=$(2)
endef

define Build/Compile
endef

ifneq ($(CONFIG_TARGET_realtek_rtd129x_giraffe-emmc),)
CONFIG_UART_HW_FLOWCTRL := y
endif

Package/rtkbt-firmware-rtl8822beh = $(call Package/rtk-btfw-default,Realtek RTL8822B UART BT Firmware,kmod-rtk_btuart)
define Package/rtkbt-firmware-rtl8822beh/install
	$(INSTALL_DIR) $(1)/lib/firmware/rtlbt
	$(INSTALL_DATA) $(UART_FW_DIR)/8822BE/rtl8822b_fw $(1)/lib/firmware/rtlbt
ifneq ($(CONFIG_UART_HW_FLOWCTRL),)
	$(INSTALL_DATA) $(UART_FW_DIR)/8822BE/rtl8822b_config_flowcontrol $(1)/lib/firmware/rtlbt/rtl8822b_config
else
	$(INSTALL_DATA) $(UART_FW_DIR)/8822BE/rtl8822b_config $(1)/lib/firmware/rtlbt/rtl8822b_config
endif
endef
$(eval $(call BuildPackage,rtkbt-firmware-rtl8822beh))

Package/rtkbt-firmware-rtl8822bu = $(call Package/rtk-btfw-default,Realtek RTL8822B USB BT Firmware,kmod-rtk_btusb)
define Package/rtkbt-firmware-rtl8822bu/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(USB_FW_DIR)/8822BU/* $(1)/lib/firmware
endef
$(eval $(call BuildPackage,rtkbt-firmware-rtl8822bu))

Package/rtkbt-firmware-rtl8821cu = $(call Package/rtk-btfw-default,Realtek RTL8821C USB BT Firmware,kmod-rtk_btusb)
define Package/rtkbt-firmware-rtl8821cu/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(USB_FW_DIR)/8821CU/* $(1)/lib/firmware
endef
$(eval $(call BuildPackage,rtkbt-firmware-rtl8821cu))

Package/rtkbt-firmware-rtl8822cu = $(call Package/rtk-btfw-default,Realtek RTL8822C USB BT Firmware,kmod-rtk_btusb)
define Package/rtkbt-firmware-rtl8822cu/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(USB_FW_DIR)/8822CU/* $(1)/lib/firmware
endef
$(eval $(call BuildPackage,rtkbt-firmware-rtl8822cu))

Package/rtkbt-firmware-rtl8822cs = $(call Package/rtk-btfw-default,Realtek RTL8822C UART BT Firmware,kmod-rtk_btuart)
define Package/rtkbt-firmware-rtl8822cs/install
	$(INSTALL_DIR) $(1)/lib/firmware/rtlbt
	$(INSTALL_DATA) $(UART_FW_DIR)/8822CS/rtl8822cs_fw $(1)/lib/firmware/rtlbt
ifneq ($(CONFIG_UART_HW_FLOWCTRL),)
	$(INSTALL_DATA) $(UART_FW_DIR)/8822CS/rtl8822cs_config_flowcontrol $(1)/lib/firmware/rtlbt/rtl8822cs_config
else
	$(INSTALL_DATA) $(UART_FW_DIR)/8822CS/rtl8822cs_config $(1)/lib/firmware/rtlbt/rtl8822cs_config
endif	
endef
$(eval $(call BuildPackage,rtkbt-firmware-rtl8822cs))
