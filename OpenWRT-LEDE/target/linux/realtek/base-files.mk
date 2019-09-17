ifeq ($(CONFIG_RTK_BOARD_MTD_LAYOUT),y)
define Package/base-files/install-target
	cp $(PLATFORM_DIR)/image/upgrade/platform.sh $(1)/lib/upgrade/
	sed -i 's|\(ROOTFS_SIZE=\).*|\1$(CONFIG_RTK_MTD_ROOTFS_SIZE)|g' $(1)/lib/upgrade/platform.sh
	sed -i 's|\(UBOOT_SIZE=\).*|\1$(CONFIG_RTK_MTD_UBOOT_SIZE)|g' $(1)/lib/upgrade/platform.sh
	sed -i 's|\(KERNEL_SIZE=\).*|\1$(CONFIG_RTK_MTD_KERNEL_SIZE)|g' $(1)/lib/upgrade/platform.sh
	sed -i 's|\(DTB_SIZE=\).*|\1$(CONFIG_RTK_MTD_DTB_SIZE)|g' $(1)/lib/upgrade/platform.sh
	sed -i 's|\(AFW_SIZE=\).*|\1$(CONFIG_RTK_MTD_AFW_SIZE)|g' $(1)/lib/upgrade/platform.sh
	sed -i 's|\(LOGO_SIZE=\).*|\1$(CONFIG_RTK_MTD_LOGO_SIZE)|g' $(1)/lib/upgrade/platform.sh
endef
endif
ifeq ($(CONFIG_RTK_BOARD_FWDESC_LAYOUT),y)
ifneq ($(CONFIG_PACKAGE_rtk-init),y)
define Package/base-files/install-target
	ln -s /sbin/init $(1)/etc/init
endef
else ifeq ($(CONFIG_PACKAGE_kmod-android),y)
define Package/base-files/install-target
	mkdir -p $(1)/mnt/android
	mkdir -p $(1)/android
endef
endif
endif
