include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk
include $(CURDIR)/audiofw.mk
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
ifneq ($(CONFIG_PACKAGE_rtk-bluecore),)
  ifeq ("$(CONFIG_RTK_BOARD_CHIP_1619)","y")
    AFW_CMD = cp $(TOPDIR)/../Packages/fw/bluecore.audio/bluecore.audio-RTD1619.zip $(TARGET_IMAGE_DIR)/bluecore.audio.zip
  else
    AFW_CMD = cp $(TOPDIR)/../Packages/fw/bluecore.audio/bluecore.audio.zip $(TARGET_IMAGE_DIR)/bluecore.audio.zip
  endif
endif

ifeq ("$(layout_type)","spi")
ifeq ("$(CONFIG_RTK_BLUECORE)","y")
define Image/Build/rtkimg
 	rm -f $(TOPDIR)/../Toolchain; ln -sf $(TOPDIR)/../Toolchain ./rtk-imagefile/Toolchain
	cp $(FEED_DIR)/feeds.conf.spi $(FEED_DIR)/feeds.conf
ifeq ("$(CONFIG_RTK_NAS_VIDEO)","y")
	sed -i 's/burn_bootlogo=n/burn_bootlogo=y/g' $(FEED_DIR)/feeds.conf
else
	sed -i 's/^bootlogo_file=[^\n]\+/bootlogo_file=/g' $(FEED_DIR)/feeds.conf
	sed -i 's/^bootlogo_zone=[^\n]\+/bootlogo_zone=\$\(\(0\)\)/g' $(FEED_DIR)/feeds.conf
	sed -i 's/burn_bootlogo=y/burn_bootlogo=n/g' $(FEED_DIR)/feeds.conf
endif
	$(call Build/kernel-dtb,$(DTS_PREFIX)$(DTS_SUFFIX))
	cp $(DTB_FILE) $(FEED_DIR)/normal.dtb
	$(call Build/kernel-dtb,$(RESCUE_DTS_PREFIX)$(RESCUE_DTS_SUFFIX))
	cp $(DTB_FILE) $(FEED_DIR)/rescue.dtb
	cp $(KDIR)/$$(KERNEL_NAME) $(FEED_DIR)/Image
  ifeq ("$(CONFIG_RTK_BOARD_CHIP_1619)","y")
   unzip -o $(TOPDIR)/../Packages/fw/bluecore.audio/bluecore.audio-RTD1619.zip  bluecore.audio.slim -d $(FEED_DIR)
  else
	 unzip -o $(TOPDIR)/../Packages/fw/bluecore.audio/bluecore.audio.zip  bluecore.audio.slim -d $(FEED_DIR)
  endif
	mv $(FEED_DIR)/bluecore.audio.slim $(FEED_DIR)/bluecore.audio
	cp $(RESCUE_ROOTFS) $(FEED_DIR)
	printf "%s=%s\n" "BUILD_TARGET_PROFILE" "$(SDK)$(CONFIG_TARGET_PROFILE)" >> $(BUILD_DIR)/root-realtek/etc/build_info; \
		printf "%s='%s'\n" "BUILD_LAYOUT_TYPE" "$(layout_type)" >> $(BUILD_DIR)/root-realtek/etc/build_info; \
		printf "%s='%s'\n" "BUILD_ARCH" "$(SDK)$(CONFIG_ARCH)" >> $(BUILD_DIR)/root-realtek/etc/build_info; \
		printf "%s='%s'\n" "BUILD_CPU_TYPE" "$(SDK)$(CONFIG_CPU_TYPE)" >> $(BUILD_DIR)/root-realtek/etc/build_info; \
		printf "%s='%s'\n" "BUILD_RTK_BOARD_CHIP" "$(SDK)$(CONFIG_RTK_BOARD_CHIP)" >> $(BUILD_DIR)/root-realtek/etc/build_info; \
		printf "%s='%s'\n" "BUILD_RTK_BOARD_CHIP_CODENAME" "$(SDK)$(CONFIG_RTK_BOARD_CHIP_CODENAME)" >> $(BUILD_DIR)/root-realtek/etc/build_info; \
		printf "%s='%s'\n" "BUILD_RTK_BOARD_DDR_SIZE" "$(SDK)$(CONFIG_RTK_BOARD_DDR_SIZE)" >> $(BUILD_DIR)/root-realtek/etc/build_info;
	cp $(BUILD_DIR)/root-realtek/etc/build_info $(STAGING_DIR)/root-realtek/etc/build_info
	( cd $(IMAGE_BUILDER); $(IMAGE_BUILDER)/build_image.sh $(IMAGE_BUILDER)/feed $(CHIP)_$(layout_type) || exit 1; cd -)
	cp $(IMAGE_BUILDER)/install.img $(BIN_DIR)
	rm -rf $(BIN_DIR)/rescue; cp $(IMAGE_BUILDER)/workspace/rescue $(BIN_DIR) -a
endef
else
define Image/Build/rtkimg
 	rm -f $(TOPDIR)/../Toolchain; ln -sf $(TOPDIR)/../Toolchain ./rtk-imagefile/Toolchain
	cp $(FEED_DIR)/feeds.conf.spi $(FEED_DIR)/feeds.conf
ifeq ("$(CONFIG_RTK_NAS_VIDEO)","y")
	sed -i 's/burn_bootlogo=n/burn_bootlogo=y/g' $(FEED_DIR)/feeds.conf
else
	sed -i 's/^bootlogo_file=[^\n]\+/bootlogo_file=/g' $(FEED_DIR)/feeds.conf
	sed -i 's/^bootlogo_zone=[^\n]\+/bootlogo_zone=\$\(\(0\)\)/g' $(FEED_DIR)/feeds.conf
	sed -i 's/burn_bootlogo=y/burn_bootlogo=n/g' $(FEED_DIR)/feeds.conf
endif
	sed -i 's/bluecore_zone=/#bluecore_zone=\nbluecore_zone=0\n#/g'  $(FEED_DIR)/feeds.conf
	sed -i 's/bluecore_file=bluecore.audio/bluecore_file=/g'  $(FEED_DIR)/feeds.conf
	sed -i 's/burn_bluecore=y/burn_bluecore=n/g'  $(FEED_DIR)/feeds.conf
	$(call Build/kernel-dtb,$(DTS_PREFIX)$(DTS_SUFFIX))
	cp $(DTB_FILE) $(FEED_DIR)/normal.dtb
	$(call Build/kernel-dtb,$(RESCUE_DTS_PREFIX)$(RESCUE_DTS_SUFFIX))
	cp $(DTB_FILE) $(FEED_DIR)/rescue.dtb
	cp $(KDIR)/$$(KERNEL_NAME) $(FEED_DIR)/Image
	cp $(RESCUE_ROOTFS) $(FEED_DIR)
	printf "%s=%s\n" "BUILD_TARGET_PROFILE" "$(SDK)$(CONFIG_TARGET_PROFILE)" >> $(BUILD_DIR)/root-realtek/etc/build_info; \
		printf "%s='%s'\n" "BUILD_LAYOUT_TYPE" "$(layout_type)" >> $(BUILD_DIR)/root-realtek/etc/build_info; \
		printf "%s='%s'\n" "BUILD_ARCH" "$(SDK)$(CONFIG_ARCH)" >> $(BUILD_DIR)/root-realtek/etc/build_info; \
		printf "%s='%s'\n" "BUILD_CPU_TYPE" "$(SDK)$(CONFIG_CPU_TYPE)" >> $(BUILD_DIR)/root-realtek/etc/build_info; \
		printf "%s='%s'\n" "BUILD_RTK_BOARD_CHIP" "$(SDK)$(CONFIG_RTK_BOARD_CHIP)" >> $(BUILD_DIR)/root-realtek/etc/build_info; \
		printf "%s='%s'\n" "BUILD_RTK_BOARD_CHIP_CODENAME" "$(SDK)$(CONFIG_RTK_BOARD_CHIP_CODENAME)" >> $(BUILD_DIR)/root-realtek/etc/build_info; \
		printf "%s='%s'\n" "BUILD_RTK_BOARD_DDR_SIZE" "$(SDK)$(CONFIG_RTK_BOARD_DDR_SIZE)" >> $(BUILD_DIR)/root-realtek/etc/build_info;
	cp $(BUILD_DIR)/root-realtek/etc/build_info $(STAGING_DIR)/root-realtek/etc/build_info
	( cd $(IMAGE_BUILDER); $(IMAGE_BUILDER)/build_image.sh $(IMAGE_BUILDER)/feed $(CHIP)_$(layout_type) || exit 1; cd -)
	cp $(IMAGE_BUILDER)/install.img $(BIN_DIR)
	rm -rf $(BIN_DIR)/rescue; cp $(IMAGE_BUILDER)/workspace/rescue $(BIN_DIR) -a
endef
endif
else
ifeq ("$(layout_type)","emmc")
ifeq ("$(CONFIG_RTK_BLUECORE)","y")
define Image/Build/rtkimg
 	rm -f $(TOPDIR)/../Toolchain; ln -sf $(TOPDIR)/../Toolchain ./rtk-imagefile/Toolchain
	$(TAR) -cjBpf $(FEED_DIR)/root.$(layout_type).tar.bz2  --numeric-owner --owner=0 --group=0 -C $(TARGET_DIR)/ .
	rm -rf $(FEED_DIR)/root-realtek
	mkdir $(FEED_DIR)/root-realtek
	$(TAR) -xjBpf $(FEED_DIR)/root.$(layout_type).tar.bz2  --numeric-owner --owner=0 --group=0 -C $(FEED_DIR)/root-realtek
	printf "%s=%s\n" "BUILD_TARGET_PROFILE" "$(SDK)$(CONFIG_TARGET_PROFILE)" >> $(FEED_DIR)/root-realtek/etc/build_info; \
		printf "%s='%s'\n" "BUILD_LAYOUT_TYPE" "$(layout_type)" >> $(FEED_DIR)/root-realtek/etc/build_info; \
		printf "%s='%s'\n" "BUILD_ARCH" "$(SDK)$(CONFIG_ARCH)" >> $(FEED_DIR)/root-realtek/etc/build_info; \
		printf "%s='%s'\n" "BUILD_CPU_TYPE" "$(SDK)$(CONFIG_CPU_TYPE)" >> $(FEED_DIR)/root-realtek/etc/build_info; \
		printf "%s='%s'\n" "BUILD_RTK_BOARD_CHIP" "$(SDK)$(CONFIG_RTK_BOARD_CHIP)" >> $(FEED_DIR)/root-realtek/etc/build_info; \
		printf "%s='%s'\n" "BUILD_RTK_BOARD_CHIP_CODENAME" "$(SDK)$(CONFIG_RTK_BOARD_CHIP_CODENAME)" >> $(FEED_DIR)/root-realtek/etc/build_info; \
		printf "%s='%s'\n" "BUILD_RTK_BOARD_DDR_SIZE" "$(SDK)$(CONFIG_RTK_BOARD_DDR_SIZE)" >> $(FEED_DIR)/root-realtek/etc/build_info;
	cp $(FEED_DIR)/root-realtek/etc/build_info $(BUILD_DIR)/root-realtek/etc/build_info
	cp $(FEED_DIR)/root-realtek/etc/build_info $(STAGING_DIR)/root-realtek/etc/build_info

	cp $(FEED_DIR)/feeds.conf.emmc $(FEED_DIR)/feeds.conf
ifeq ("$(CONFIG_RTK_NAS_VIDEO)","y")
	sed -i 's/bluecore_zone=$$$$((0x13A000))/bluecore_zone=$$$$((0x400000))/g'  $(FEED_DIR)/feeds.conf
	sed -i 's/burn_bootlogo=n/burn_bootlogo=y/g' $(FEED_DIR)/feeds.conf
else
	sed -i 's/^bootlogo_file=[^\n]\+/bootlogo_file=/g' $(FEED_DIR)/feeds.conf
	sed -i 's/^bootlogo_zone=[^\n]\+/bootlogo_zone=\$\(\(0\)\)/g' $(FEED_DIR)/feeds.conf
	sed -i 's/burn_bootlogo=y/burn_bootlogo=n/g' $(FEED_DIR)/feeds.conf
endif
	$(call Build/kernel-dtb,$(DTS_PREFIX)$(DTS_SUFFIX))
	cp $(DTB_FILE) $(FEED_DIR)/normal.dtb
	$(call Build/kernel-dtb,$(RESCUE_DTS_PREFIX)$(RESCUE_DTS_SUFFIX))
	cp $(DTB_FILE) $(FEED_DIR)/rescue.dtb
	cp $(KDIR)/$$(KERNEL_NAME) $(FEED_DIR)/Image
ifeq ("$(CONFIG_RTK_NAS_VIDEO)","y")
  ifeq ("$(CONFIG_RTK_BOARD_CHIP_1619)","y")
    unzip -o $(TOPDIR)/../Packages/fw/bluecore.audio/bluecore-RTD1619.audio.zip  bluecore.audio.CVBS_On -d $(FEED_DIR)
  else
    unzip -o $(TOPDIR)/../Packages/fw/bluecore.audio/bluecore.audio.zip  bluecore.audio.CVBS_On -d $(FEED_DIR)
  endif
	mv $(FEED_DIR)/bluecore.audio.CVBS_On $(FEED_DIR)/bluecore.audio
else
  ifeq ("$(CONFIG_RTK_BOARD_CHIP_1619)","y")
    unzip -o $(TOPDIR)/../Packages/fw/bluecore.audio/bluecore-RTD1619.audio.zip  bluecore.audio.slim -d $(FEED_DIR)
  else
    unzip -o $(TOPDIR)/../Packages/fw/bluecore.audio/bluecore.audio.zip  bluecore.audio.slim -d $(FEED_DIR)
  endif
	mv $(FEED_DIR)/bluecore.audio.slim $(FEED_DIR)/bluecore.audio
endif
ifeq ("$(CONFIG_PACKAGE_libiconv-full)","y")
	echo -e "\nexport LD_PRELOAD=/usr/lib/preloadable_libiconv.so" >> $(FEED_DIR)/root-realtek/etc/profile
endif
ifeq ("$(CONFIG_PACKAGE_weston)","y")
	echo -e "\nexport XDG_RUNTIME_DIR=/tmp" >> $(FEED_DIR)/root-realtek/etc/profile
endif
	cp $(RESCUE_ROOTFS) $(FEED_DIR)
	( cd $(IMAGE_BUILDER); $(IMAGE_BUILDER)/build_image.sh $(IMAGE_BUILDER)/feed $(CHIP)_$(layout_type) || exit 1; cd -)
	cp $(IMAGE_BUILDER)/install.img $(BIN_DIR)
	rm -rf $(BIN_DIR)/rescue; cp $(IMAGE_BUILDER)/workspace/rescue $(BIN_DIR) -a
endef
else
define Image/Build/rtkimg
 	rm -f $(TOPDIR)/../Toolchain; ln -sf $(TOPDIR)/../Toolchain ./rtk-imagefile/Toolchain
	$(TAR) -cjBpf $(FEED_DIR)/root.$(layout_type).tar.bz2  --numeric-owner --owner=0 --group=0 -C $(TARGET_DIR)/ .
	rm -rf $(FEED_DIR)/root-realtek
	mkdir $(FEED_DIR)/root-realtek
	$(TAR) -xjBpf $(FEED_DIR)/root.$(layout_type).tar.bz2  --numeric-owner --owner=0 --group=0 -C $(FEED_DIR)/root-realtek
	printf "%s=%s\n" "BUILD_TARGET_PROFILE" "$(SDK)$(CONFIG_TARGET_PROFILE)" >> $(FEED_DIR)/root-realtek/etc/build_info; \
		printf "%s='%s'\n" "BUILD_LAYOUT_TYPE" "$(layout_type)" >> $(FEED_DIR)/root-realtek/etc/build_info; \
		printf "%s='%s'\n" "BUILD_ARCH" "$(SDK)$(CONFIG_ARCH)" >> $(FEED_DIR)/root-realtek/etc/build_info; \
		printf "%s='%s'\n" "BUILD_CPU_TYPE" "$(SDK)$(CONFIG_CPU_TYPE)" >> $(FEED_DIR)/root-realtek/etc/build_info; \
		printf "%s='%s'\n" "BUILD_RTK_BOARD_CHIP" "$(SDK)$(CONFIG_RTK_BOARD_CHIP)" >> $(FEED_DIR)/root-realtek/etc/build_info; \
		printf "%s='%s'\n" "BUILD_RTK_BOARD_CHIP_CODENAME" "$(SDK)$(CONFIG_RTK_BOARD_CHIP_CODENAME)" >> $(FEED_DIR)/root-realtek/etc/build_info; \
		printf "%s='%s'\n" "BUILD_RTK_BOARD_DDR_SIZE" "$(SDK)$(CONFIG_RTK_BOARD_DDR_SIZE)" >> $(FEED_DIR)/root-realtek/etc/build_info;
	cp $(FEED_DIR)/root-realtek/etc/build_info $(BUILD_DIR)/root-realtek/etc/build_info
	cp $(FEED_DIR)/root-realtek/etc/build_info $(STAGING_DIR)/root-realtek/etc/build_info

	cp $(FEED_DIR)/feeds.conf.emmc $(FEED_DIR)/feeds.conf
ifeq ("$(CONFIG_RTK_NAS_VIDEO)","y")
	sed -i 's/burn_bootlogo=n/burn_bootlogo=y/g' $(FEED_DIR)/feeds.conf
else
	sed -i 's/^bootlogo_file=[^\n]\+/bootlogo_file=/g' $(FEED_DIR)/feeds.conf
	sed -i 's/^bootlogo_zone=[^\n]\+/bootlogo_zone=\$\(\(0\)\)/g' $(FEED_DIR)/feeds.conf
	sed -i 's/burn_bootlogo=y/burn_bootlogo=n/g' $(FEED_DIR)/feeds.conf
endif
	sed -i 's/bluecore_zone=/#bluecore_zone=\nbluecore_zone=0\n#/g'  $(FEED_DIR)/feeds.conf
	sed -i 's/bluecore_file=bluecore.audio/bluecore_file=/g'  $(FEED_DIR)/feeds.conf
	sed -i 's/burn_bluecore=y/burn_bluecore=n/g'  $(FEED_DIR)/feeds.conf
	$(call Build/kernel-dtb,$(DTS_PREFIX)$(DTS_SUFFIX))
	cp $(DTB_FILE) $(FEED_DIR)/normal.dtb
	$(call Build/kernel-dtb,$(RESCUE_DTS_PREFIX)$(RESCUE_DTS_SUFFIX))
	cp $(DTB_FILE) $(FEED_DIR)/rescue.dtb
	cp $(KDIR)/$$(KERNEL_NAME) $(FEED_DIR)/Image
	cp $(RESCUE_ROOTFS) $(FEED_DIR)
	( cd $(IMAGE_BUILDER); $(IMAGE_BUILDER)/build_image.sh $(IMAGE_BUILDER)/feed $(CHIP)_$(layout_type) || exit 1; cd -)
	cp $(IMAGE_BUILDER)/install.img $(BIN_DIR)
	rm -rf $(BIN_DIR)/rescue; cp $(IMAGE_BUILDER)/workspace/rescue $(BIN_DIR) -a
endef
endif
else
define Image/Build/rtkimg
		rm -rf $(TARGET_IMAGE_DIR)/../components $(TARGET_IMAGE_DIR)/../tmp
		$(AFW_CMD)
		$(call Build/kernel-dtb,$(DTS_PREFIX)$(DTS_SUFFIX))
		cp $(DTB_FILE) $(TARGET_IMAGE_DIR)/android.$(layout_type).dtb
		$(call Build/kernel-dtb,$(RESCUE_DTS_PREFIX)$(RESCUE_DTS_SUFFIX))
		cp $(DTB_FILE) $(TARGET_IMAGE_DIR)/rescue.$(layout_type).dtb
		cp $(KDIR)/$$(KERNEL_NAME) $(TARGET_IMAGE_DIR)/$(layout_type).uImage
		$(TAR) -cjBpf $(TARGET_IMAGE_DIR)/root.$(layout_type).tar.bz2  --numeric-owner --owner=0 --group=0 -C $(TARGET_DIR)/ .

		if [ "$(layout_type)" = "nand" ]; then \
			$(SED) "s/^\(NAND_PAGE_SIZE=\).*/\1$(NAND_PAGE_SIZE)/g" $(TARGET_IMAGE_DIR)/Makefile.in; \
			$(SED) "s/^\(NAND_BLOCK_SIZE=\).*/\1$(NAND_BLOCK_SIZE)/g" $(TARGET_IMAGE_DIR)/Makefile.in; \
		fi

		(cd $(IMG_NAME)/components; \
			export ANDROID_PRODUCT_OUT="$(ANDROID_PRODUCT_OUT)"; \
			export ANDROID_HOST_OUT="$(ANDROID_HOST_OUT)"; \
			make -f Makefile.$(call qstrip,$(CONFIG_RTK_BOARD_CHIP_CODENAME)) \
			TARGET=omv layout_type=$(layout_type) layout_size=$(layout_size) \
			layout_use_emmc_swap=$(layout_use_emmc_swap) \
			ANDROID_IMGS=$(ANDROID_IMGS) install_bootloader=$(install_bootloader) \
			install_factory=$(install_factory) update_etc=$(update_etc) logger_level=$(logger_level) \
			hash_imgfile=$(hash_imgfile) verify=$(verify) \
			offline_gen=$(offline_gen) gen_install_binary=$(gen_install_binary) \
			SECURE_BOOT=$(secure_boot) efuse_key=$(efuse_key) \
			DUAL_BOOT=$(dual_boot) \
			KERNELDT_MINSIZE=$(KERNELDT_MINSIZE) RESCUEDT_MINSIZE=$(RESCUEDT_MINSIZE) \
			LINUX_MINSIZE=$(LINUX_MINSIZE) RESCUEROOTFS_MINSIZE=$(RESCUEROOTFS_MINSIZE) \
			AUDIO_MINSIZE=$(AUDIO_MINSIZE) LOGO_MINSIZE=$(LOGO_MINSIZE) \
			chip_rev=$(chip_rev) \
			HYPERVISOR=$(HYPERVISOR) \
			F_EXT=$(F_EXT) \
			AUDIOADDR=$(AUDIOADDR) \
			install_avfile_count=$(install_avfile_count) \
			MODELNAME=$(SUBTARGET)-$(PROFILE) \
			; cd -)
		/bin/mv $(IMG_NAME)/install.img $(BIN_DIR)
endef
endif
endif
endif

define Image/Build
	$(if $(Image/Build/$(1)), \
		$(call Image/Build/$(1),$(1))
	)
endef

$(eval $(call BuildImage))
