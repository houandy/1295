include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk
#include $(CURDIR)/audiofw.mk
include $(CURDIR)/image.mk

KERNEL_NAME := Image

# default all platform image(fit) build
define Device/Default/Config
	select RTK_BOARD_MTD_LAYOUT
endef

define Device/Default
  KERNEL := kernel-bin | lzma -d20
  KERNEL_NAME := Image
ifeq ($(CONFIG_RTK_BOARD_MTD_LAYOUT),y)
  FILESYSTEMS := initrd
  KERNEL_SIZE := $(CONFIG_RTK_MTD_KERNEL_SIZE)k
  UBOOT_SIZE := $(CONFIG_RTK_MTD_UBOOT_SIZE)k
  LOGO_SIZE := $(CONFIG_RTK_MTD_LOGO_SIZE)k
  AFW_SIZE := $(CONFIG_RTK_MTD_AFW_SIZE)k
  DTB_SIZE := $(CONFIG_RTK_MTD_DTB_SIZE)k
  ROOTFS_SIZE := $(CONFIG_RTK_MTD_ROOTFS_SIZE)k
else
  FILESYSTEMS :=
endif
  BOARDNAME :=
  CONSOLE :=
  MTDPARTS :=
  CMDLINE = $$(if $$(BOARDNAME),board=$$(BOARDNAME) )$$(if $$(MTDPARTS),mtdparts=$$(MTDPARTS) )$$(if $$(CONSOLE),console=$$(CONSOLE) )
  AFW_CMD :=
  UBOOT_FILE := x
  BLOCKSIZE := 64k
  IMAGES := default
  IMAGE/uboot = append-uboot $$(UBOOT_SIZE)
  IMAGE/full = append-uboot $$(UBOOT_SIZE) | $$$$(IMAGE/default)
  IMAGE/rootfs = append-rootfs-w-hash $$(ROOTFS_SIZE)
  IMAGE/kernel = $$(IMAGE/dtb) | append-kernel-w-hash $$(KERNEL_SIZE)
  #IMAGE/dtb = append-dtb $$(DTB_SIZE)
  IMAGE/afw = append-afw $$(AFW_SIZE)
  IMAGE/logo = append-logo $$(LOGO_SIZE)
  IMAGE/pack = padding-hash | check-size $$$$(1)
endef

-include $(CHIP).mk

ifneq ($(CONFIG_PACKAGE_rtk-imagefile),)
ifneq ("$(layout_type)", "spi")
ifneq ("$(layout_type)", "emmc")
include $(IMG_NAME)/Makefile.in
endif
endif
endif
ifeq ($(CONFIG_RTK_NAS_TRANSCODE),)
  AUDIOADDR=0
endif
ifeq ($(CONFIG_RTK_BOARD_FWDESC_LAYOUT),y)
#  DTB_CMD = $(SED) "/linux,initrd/d;" $(DTS_FILE)
#  RESCUE_DTB_CMD = $(SED) "/memory.h/a\#undef ROOTFS_RESCUE_SIZE\n\#define ROOTFS_RESCUE_SIZE      (0x00400000)" $(DTS_FILE)
#ifneq ($(CONFIG_PACKAGE_rtk-bluecore),)
#  AFW_CMD = cp $(TOPDIR)/../Packages/fw/bluecore.audio/bluecore.audio.zip $(TARGET_IMAGE_DIR)/bluecore.audio.zip
#endif

define Image/Build/rtkimg/prepare-rootfs/check-initrd-zone
	@FEED_ZONE=$(shell grep initramfs_zone $(FEED_DIR)/feeds.conf | grep -o -P "0x\d+"); \
	DTS_ZONE=$(shell grep INITRD_SIZE $(DTS_FULL_DIR)/$(DTS_PREFIX)$(DTS_SUFFIX).dts | grep -o -P "0x\d+"); \
	[ -z "$$$$FEED_ZONE" ] || [ -z "$$$$DTS_ZONE" ] || \
	[[ $$$$DTS_ZONE -eq $$$$FEED_ZONE ]] || ( \
	echo "******************************************************************************" && \
	echo "ERROR: The reserved space for initrd in DTS ($$$$DTS_ZONE) is not equal to" && \
	echo "       initramfs_zone in feeds.conf ($$$$FEED_ZONE)" && \
	echo "******************************************************************************" && \
	exit 1)
endef

define Image/Build/rtkimg/prepare-rootfs/initrd
	@echo "[RTKIMG] prepare initrd"
	rm -f $(FEED_DIR)/initramfs.cpio.gz
ifeq ("$(CONFIG_RTK_ROOTFS_INITRD)","y")
	$(call Image/mkfs/initrd)
	cp $(KDIR)/root.initrd $(FEED_DIR)/initramfs.cpio.gz
	sed -i 's/storage_size=\$$$$((16\*1024\*1024))/storage_size=\$$$$((32*1024*1024))/' $(FEED_DIR)/feeds.conf
	$(call Image/Build/rtkimg/prepare-rootfs/check-initrd-zone)
else
	sed -i 's/initramfs_file=/#initramfs_file=/' $(FEED_DIR)/feeds.conf
endif
endef

define Image/Build/rtkimg/prepare-rootfs
	@echo "[RTKIMG] Prepare rootfs"
	$(call Image/Build/rtkimg/build_info)
ifeq ("$(CONFIG_PACKAGE_libiconv-full)","y")
	echo -e "\nexport LD_PRELOAD=/usr/lib/preloadable_libiconv.so" >> $(BUILD_DIR)/root-realtek/etc/profile
endif
ifeq ("$(CONFIG_PACKAGE_weston)","y")
	echo -e "\nexport XDG_RUNTIME_DIR=/tmp" >> $(BUILD_DIR)/root-realtek/etc/profile
endif
ifeq ("$(layout_type)","emmc")
	$(TAR) -cjBpf $(FEED_DIR)/root.$(layout_type).tar.bz2  --numeric-owner --owner=0 --group=0 -C $(TARGET_DIR)/ .
	rm -rf $(FEED_DIR)/root-realtek
	mkdir $(FEED_DIR)/root-realtek
	$(TAR) -xjBpf $(FEED_DIR)/root.$(layout_type).tar.bz2  --numeric-owner --owner=0 --group=0 -C $(FEED_DIR)/root-realtek
endif
	$(call Image/Build/rtkimg/prepare-rootfs/initrd)
endef

define Image/Build/rtkimg/build_info
	printf "%s=%s\n" "BUILD_TARGET_PROFILE" "$(SDK)$(CONFIG_TARGET_PROFILE)" >> $(BUILD_DIR)/root-realtek/etc/build_info; \
	printf "%s='%s'\n" "BUILD_LAYOUT_TYPE" "$(layout_type)" >> $(BUILD_DIR)/root-realtek/etc/build_info; \
	printf "%s='%s'\n" "BUILD_ARCH" "$(SDK)$(CONFIG_ARCH)" >> $(BUILD_DIR)/root-realtek/etc/build_info; \
	printf "%s='%s'\n" "BUILD_CPU_TYPE" "$(SDK)$(CONFIG_CPU_TYPE)" >> $(BUILD_DIR)/root-realtek/etc/build_info; \
	printf "%s='%s'\n" "BUILD_RTK_BOARD_CHIP" "$(SDK)$(CONFIG_RTK_BOARD_CHIP)" >> $(BUILD_DIR)/root-realtek/etc/build_info; \
	printf "%s='%s'\n" "BUILD_RTK_BOARD_CHIP_CODENAME" "$(SDK)$(CONFIG_RTK_BOARD_CHIP_CODENAME)" >> $(BUILD_DIR)/root-realtek/etc/build_info; \
	printf "%s='%s'\n" "BUILD_RTK_BOARD_DDR_SIZE" "$(SDK)$(CONFIG_RTK_BOARD_DDR_SIZE)" >> $(BUILD_DIR)/root-realtek/etc/build_info;
	cp $(BUILD_DIR)/root-realtek/etc/build_info $(STAGING_DIR)/root-realtek/etc/build_info
endef

define Image/Build/rtkimg/rtk-bluecore
	@echo "[RTKIMG] Prepare ACPU firmware"
ifeq ("$(CONFIG_PACKAGE_rtk-bluecore)","y")
	cp $(BUILD_DIR)/rtk-bluecore/release/bluecore.audio $(FEED_DIR)/bluecore.audio
else
	sed -i 's/bluecore_zone=/#bluecore_zone=\nbluecore_zone=0\n#/g' $(FEED_DIR)/feeds.conf
	sed -i 's/bluecore_file=bluecore.audio/bluecore_file=/g' $(FEED_DIR)/feeds.conf
	sed -i 's/burn_bluecore=y/burn_bluecore=n/g' $(FEED_DIR)/feeds.conf
endif
endef

define Image/Build/rtkimg/bootlogo
	@echo "[RTKIMG] Prepare Boot Logo"
ifeq ("$(CONFIG_RTK_NAS_VIDEO)","y")
	sed -i 's/burn_bootlogo=n/burn_bootlogo=y/g' $(FEED_DIR)/feeds.conf
else
	sed -i 's/^bootlogo_file=[^\n]\+/bootlogo_file=/g' $(FEED_DIR)/feeds.conf
	sed -i 's/^bootlogo_zone=[^\n]\+/bootlogo_zone=$$$$\(\(0\)\)/g' $(FEED_DIR)/feeds.conf
	sed -i 's/burn_bootlogo=y/burn_bootlogo=n/g' $(FEED_DIR)/feeds.conf
endif
endef

define Image/Build/rtkimg
	rm -f $(TOPDIR)/../Toolchain; ln -sf $(TOPDIR)/../Toolchain ./rtk-imagefile/Toolchain
	cp $(FEED_DIR)/feeds.conf.$(layout_type) $(FEED_DIR)/feeds.conf
	$(call Image/Build/rtkimg/prepare-rootfs)
	$(call Image/Build/rtkimg/rtk-bluecore)
	$(call Image/Build/rtkimg/bootlogo)

	$(call Build/kernel-dtb,$(DTS_PREFIX)$(DTS_SUFFIX))
	cp $(DTB_FILE) $(FEED_DIR)/normal.dtb
	$(call Build/kernel-dtb,$(RESCUE_DTS_PREFIX)$(RESCUE_DTS_SUFFIX))
	cp $(DTB_FILE) $(FEED_DIR)/rescue.dtb
	cp $(KDIR)/$$(KERNEL_NAME) $(FEED_DIR)/Image
	cp $(RESCUE_ROOTFS) $(FEED_DIR)
	( cd $(IMAGE_BUILDER); $(IMAGE_BUILDER)/build_image.sh $(IMAGE_BUILDER)/feed $(CHIP)_$(layout_type) || exit 1; cd -)
	cp $(IMAGE_BUILDER)/install.img $(BIN_DIR)
	rm -rf $(BIN_DIR)/rescue; cp $(IMAGE_BUILDER)/workspace/rescue $(BIN_DIR) -a
ifeq ("$(layout_type)","spi")
	cd $(TARGET_DIR); tar jcf $(BIN_DIR)/root.tar.bz2 *;cd $(TOPDIR)
endif
endef
endif

define Image/Build
	$(if $(Image/Build/$(1)), \
		$(call Image/Build/$(1),$(1))
	)
endef

$(eval $(call BuildImage))
