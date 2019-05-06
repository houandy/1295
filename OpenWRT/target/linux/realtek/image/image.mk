board_name:=$(basename $(subst -,.,$(PROFILE)))
layout_type:=$(subst .,,$(suffix $(subst -,.,$(PROFILE))))

DTS_FULL_DIR:=$(DTS_DIR)/realtek/$(SUBTARGET)

DTS_PREFIX:=$(CONFIG_RTK_BOARD_CHIP)-nas-$(board_name)
ifeq ("$(CONFIG_RTK_NAS_TRANSCODE)","y")
	DTS_PREFIX:=$(CONFIG_RTK_BOARD_CHIP)-mmnas-$(board_name)
endif
ifeq ("$(CONFIG_RTK_NAS_VIDEO)","y")
	DTS_PREFIX:=$(CONFIG_RTK_BOARD_CHIP)-videonas-$(board_name)
endif

DTS_SUFFIX:=$(if $(CONFIG_RTK_BOARD_DDR_SIZE),-$(CONFIG_RTK_BOARD_DDR_SIZE))
RESCUE_DTS_PREFIX:=$(CONFIG_RTK_BOARD_CHIP)
RESCUE_DTS_SUFFIX:=-nas-qa-rescue
#SOURCE_DTS:=$(DTS_FULL_DIR)/$(DTS_PREFIX)-$(CONFIG_RTK_BOARD_DDR_SIZE).dts
DTS_FILE:=$(DTS_FULL_DIR)/$(SUBTARGET)-openwrt.dts

DTB_FILE:=$(patsubst %.dts,%.dtb,$(DTS_FILE))
FACTORY_NAME := factory.tar
LOGO_NAME := bootfile.image
AFW_NAME := bluecore.audio

IMG_NAME:=rtk-imagefile
FEED_DIR=$(CURDIR)/$(IMG_NAME)/feed
RESCUE_ROOTFS=$(CURDIR)/$(IMG_NAME)/rescue-rootfs/rescue_rootfs.cpio.gz
IMAGE_BUILDER=$(CURDIR)/$(IMG_NAME)
TARGET_IMAGE_DIR:=$(CURDIR)/$(IMG_NAME)/components/packages/omv
IMAGE_DIR:=$(CURDIR)/files
IMAGE_BIN_DIR:=$(CURDIR)/bin
define padding-sha256-cmd
	dd if=$(1) $(if $(3),bs=$(3) conv=sync) > $(1).pad && \
	head -c $(if $(3),-32,-0) $(1).pad | openssl dgst -sha256 -binary > $(1).chk && \
	head -c $(if $(3),-32,-0) $(1).pad $(if $(2),>> $(2),> $(1)) &&  \
	cat $(1).chk >> $(if $(2),$(2),$(1)) && \
	rm -f $(1).pad $(1).chk
endef

# Args: source destination alignment(opt)
# Use $@ as src and dest if no args
define Build/padding-hash
	$(call locked, \
		$(call padding-sha256-cmd,$(if $(1),$(1),$@),$(2),$(3)),\
		$(SUBTARGET))
endef

define Build/kernel-dtb
	$(CP) $(DTS_FULL_DIR)/$(1).dts $(DTS_FILE)
	$(if $(findstring rescue,$(1)),$(RESCUE_DTB_CMD),$(DTB_CMD))
	$(call Image/BuildDTB,$(DTS_FILE),$(DTB_FILE),,"-p 8192")
endef

define Build/append-dtb
	$(call Build/kernel-dtb,$(if $(word 2,$(1)),$(word 2,$(1)),$(DTS_PREFIX))$(DTS_SUFFIX))
	$(call Build/padding-hash,$(DTB_FILE),$@,$(word 1,$(1)))
endef

define Build/append-env
	dd if=/dev/zero bs=$(1) conv=sync count=1 >> $@
endef

define Build/append-factory
	mkdir -p tmp/factory
	[ -z "`ls -A $(IMAGE_DIR)/factory/`" ] || $(CP) $(IMAGE_DIR)/factory/* tmp/factory/
	$(TAR) cf $(FACTORY_NAME) tmp/factory
	$(IMAGE_BIN_DIR)/rtk_factory $(FACTORY_NAME)
	$(call Build/padding-hash,$(FACTORY_NAME),$@,$(1))
	rm -rf tmp/factory $(FACTORY_NAME)
endef

define Build/append-uboot
	$(call Build/padding-hash,$(UBOOT_FILE),$@,$(1))
endef

define Build/append-logo
	$(CP) $(IMAGE_DIR)/$(LOGO_NAME) .
	$(STAGING_DIR_HOST)/bin/lzma e $(LOGO_NAME) -lc1 -lp2 -pb2 $(LOGO_NAME).lzma
	$(call Build/padding-hash,$(LOGO_NAME).lzma,$@,$(1))
	rm -f $(LOGO_NAME) $(LOGO_NAME).lzma
endef

define Build/append-afw
	$(AFW_CMD)
	$(STAGING_DIR_HOST)/bin/lzma e $(AFW_NAME) -lc1 -lp2 -pb2 $(AFW_NAME).lzma
	$(call Build/padding-hash,$(AFW_NAME).lzma,$@,$(1))
	rm -f $(AFW_NAME) $(AFW_NAME).lzma
endef

define Build/append-kernel-w-hash
	$(call Build/padding-hash,$(word 1,$^),$@,$(1))
endef

define Build/append-rootfs-w-hash
	$(call Build/padding-hash,$(word 2,$^),$@,$(1))
endef
