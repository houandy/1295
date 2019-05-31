RTK_MENU:=Realtek kernel options

define KernelPackage/rtk-spi
  SUBMENU:=$(RTK_MENU)
  TITLE:=Realtek SPI NOR flash driver
  KCONFIG:= \
	CONFIG_MTD=y \
	CONFIG_MTD_BLKDEVS=y \
	CONFIG_MTD_BLOCK=y \
	CONFIG_MTD_CFI=n \
	CONFIG_MTD_CMDLINE_PARTS=y \
	CONFIG_MTD_COMPLEX_MAPPINGS=n \
	CONFIG_MTD_NAND=n \
	CONFIG_MTD_NAND_IDS=n \
	CONFIG_MTD_OF_PARTS=y \
	CONFIG_MTD_RTK_SFC=y \
	CONFIG_MTD_RTK_SFC_DEBUG=n \
	CONFIG_MTD_RTK_SFC_READ_MD=y \
	CONFIG_MTD_RTK_SFC_WRITE_MD=y \
	CONFIG_MTD_RTK_SFC_PSTORE=y \
	CONFIG_PSTORE=y \

  FILES:=
  AUTOLOAD:=
  DEPENDS:=@TARGET_realtek @DEFAULT_kmod-rtk-spi
endef

define KernelPackage/rtk-spi/description
  This package contains the Realtek SPI NOR flash driver.
endef

$(eval $(call KernelPackage,rtk-spi))

define KernelPackage/rtk-nand
  SUBMENU:=$(RTK_MENU)
  TITLE:=Realtek NAND flash driver
  KCONFIG:= \
	CONFIG_MTD=y \
	CONFIG_MTD_BLOCK=y \
	CONFIG_MTD_BLOCK_RO=n \
	CONFIG_MTD_CFI=n \
	CONFIG_MTD_CMDLINE_PARTS=y \
	CONFIG_MTD_COMPLEX_MAPPINGS=n \
	CONFIG_MTD_NAND=y \
	CONFIG_MTD_NAND_ECC=y \
	CONFIG_MTD_NAND_IDS=y \
	CONFIG_MTD_OF_PARTS=y \
	CONFIG_MTD_SM_COMMON=n \
	CONFIG_MTD_UBI=y \
	CONFIG_MTD_UBI_BEB_LIMIT=20 \
	CONFIG_MTD_UBI_BLOCK=y \
	CONFIG_MTD_UBI_FASTMAP=n \
	CONFIG_MTD_UBI_GLUEBI=n \
	CONFIG_MTD_UBI_WL_THRESHOLD=1024 \
	CONFIG_OF_MTD=y \
	CONFIG_UBIFS_FS=y \
	CONFIG_UBIFS_FS_ADVANCED_COMPR=n \
	CONFIG_UBIFS_FS_LZO=y \
	CONFIG_UBIFS_FS_ZLIB=y
  FILES:=
  AUTOLOAD:=
  DEPENDS:=@TARGET_realtek @DEFAULT_kmod-rtk-nand
endef

define KernelPackage/rtk-nand/config
	select NAND_SUPPORT
	select USES_UBIFS
endef

define KernelPackage/rtk-nand/description
  This package contains the Realtek NAND flash driver.
endef

$(eval $(call KernelPackage,rtk-nand))

define KernelPackage/rtk-emmc
  SUBMENU:=$(RTK_MENU)
  TITLE:=Realtek eMMC driver
  KCONFIG:= \
	CONFIG_MMC=y \
	CONFIG_MMC_BLOCK=y \
	CONFIG_MMC_EMBEDDED_SDIO=n \
	CONFIG_MMC_PARANOID_SD_INIT=n \
	CONFIG_MMC_RTK_EMMC=y \
	CONFIG_MMC_RTKEMMC_JIFFY_NOT_WORK_ON_1_LAYER_FPGA=n \
	CONFIG_MMC_RTK_SDMMC=y \
	CONFIG_MMC_RTK_SDMMC_DEBUG=n \
	CONFIG_MMC_SDHCI=n \
	CONFIG_MMC_SIMULATE_MAX_SPEED=n \
	CONFIG_MMC_TIFM_SD=n \
	CONFIG_BLK_DEV_LOOP=y \
	CONFIG_F2FS_CHECK_FS=n \
	CONFIG_F2FS_FS=y \
	CONFIG_F2FS_FS_SECURITY=n \
	CONFIG_F2FS_FS_XATTR=y \
	CONFIG_F2FS_STAT_FS=y \

  FILES:=
  AUTOLOAD:=
  DEPENDS:=@TARGET_realtek @DEFAULT_kmod-rtk-emmc
endef

define KernelPackage/rtk-emmc/config
	depends on !PACKAGE_kmod-rtk-nand
endef

define KernelPackage/rtk-emmc/description
  This package enables the Realtek eMMC driver.
endef

$(eval $(call KernelPackage,rtk-emmc))

define KernelPackage/rtl8169soc
  SUBMENU:=$(RTK_MENU)
  TITLE:=Realtek Gigabit Ethernet driver
  DEFAULT:=y
  KCONFIG:= \
	CONFIG_R8169SOC=y \
	CONFIG_NET_VENDOR_REALTEK=y
  FILES:=
  AUTOLOAD:=
  DEPENDS:=@TARGET_realtek
endef

define KernelPackage/rtl8169soc/description
  This package contains the Realtek Gigibit Ethernet driver
endef

define KernelPackage/rtl8169soc/config
	depends on !PACKAGE_kmod-rtd1295hwnat
endef

$(eval $(call KernelPackage,rtl8169soc))

define KernelPackage/rtl8125
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=Realtek R8125 PCI-E 2.5 Gigabit Ethernet driver
  DEPENDS:=@TARGET_realtek @PCI_SUPPORT +kmod-mii
  KCONFIG:=CONFIG_R8125
  FILES:=$(LINUX_DIR)/drivers/net/ethernet/realtek/r8125/r8125.ko
  AUTOLOAD:=$(call AutoProbe,r8125)
endef

define KernelPackage/rtl8125/description
  This package contains the Realtek R8125 PCI-E 2.5 Gigibit Ethernet driver
endef

$(eval $(call KernelPackage,rtl8125))

define KernelPackage/rtl8168
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=Realtek R8168 PCI-E Gigabit Ethernet driver
  DEPENDS:=@TARGET_realtek @PCI_SUPPORT +kmod-mii
  KCONFIG:=CONFIG_R8168
  FILES:=$(LINUX_DIR)/drivers/net/ethernet/realtek/r8168/r8168.ko
  AUTOLOAD:=$(call AutoProbe,r8168)
endef

define KernelPackage/rtl8168/description
  This package contains the Realtek R8168 PCI-E Gigibit Ethernet driver
endef

$(eval $(call KernelPackage,rtl8168))

define KernelPackage/rtl8168_pg
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=Realtek R8168 PCI-E Gigabit Ethernet PG driver
  DEPENDS:=@TARGET_realtek @PCI_SUPPORT +kmod-mii
  KCONFIG:=CONFIG_R8168_PG
  FILES:=$(LINUX_DIR)/drivers/net/ethernet/realtek/r8168_pg/pgdrv.ko
  AUTOLOAD:=$(call AutoProbe,r8168_pg)
endef

define KernelPackage/rtl8168_pg/description
  This package contains the Realtek R8168 PCI-E Gigibit Ethernet PG driver
endef

$(eval $(call KernelPackage,rtl8168_pg))

define KernelPackage/android
  SUBMENU:=$(RTK_MENU)
  TITLE:=Extra options for Android
  KCONFIG:= \
	CONFIG_ANDROID_BINDER_IPC=y \
	CONFIG_ANDROID_BINDER_IPC_32BIT=y \
	CONFIG_AUDIT=y \
	CONFIG_ASHMEM=y \
	CONFIG_ANDROID_LOW_MEMORY_KILLER=y \
	CONFIG_ANDROID_LOW_MEMORY_KILLER_AUTODETECT_OOM_ADJ_VALUES=y \
	CONFIG_CC_STACKPROTECTOR=y \
	CONFIG_CC_STACKPROTECTOR_STRONG=y \
	CONFIG_HARDENED_USERCOPY=y \
	CONFIG_MEDIA_SUPPORT=y \
	CONFIG_FB=y \
	CONFIG_FB_RTK=y \
	CONFIG_RTK_HDMITX=y \
	CONFIG_RTK_DPTX=y \
	CONFIG_SECURITY=y \
	CONFIG_SECURITYFS=y \
	CONFIG_SECURITY_NETWORK=y \
	CONFIG_SECURITY_PATH=y \
	CONFIG_SECURITY_SELINUX=y \
	CONFIG_SECURITY_SELINUX_CHECKREQPROT_VALUE=1 \
	CONFIG_DEFAULT_SECURITY_SELINUX=y \
	CONFIG_ZRAM=y \
	CONFIG_TEE=y \
	CONFIG_INPUT_MOUSEDEV=y \
	CONFIG_INPUT_EVDEV=y \
	CONFIG_INPUT_KEYRESET=y \
	CONFIG_INPUT_KEYCOMBO=y \
	CONFIG_INPUT_KEYBOARD=y \
	CONFIG_KEYBOARD_ATKBD=y \
	CONFIG_INPUT_UINPUT=y \
	CONFIG_INPUT_GPIO=y \
	CONFIG_USB_HID=y \
	CONFIG_USB_HIDDEV=y \

  FILES:=
  AUTOLOAD:=
  DEPENDS:=@TARGET_realtek_rtd16xx +rtk-init
endef

define KernelPackage/android/description
 Other kernel builtin options for Android
endef

$(eval $(call KernelPackage,android))

# Add NF_CONNTRACK and IP6_NF_IPTABLES for NETFILTER_XT_MATCH_SOCKET
define KernelPackage/ipt-android
  SUBMENU:=$(RTK_MENU)
  TITLE:=Extra Netfilter options for Android
  KCONFIG:= \
	CONFIG_NFT_COMPAT=n \
	CONFIG_NF_CT_NETLINK_HELPER=n \
	CONFIG_NF_TABLES_ARP=n \
	CONFIG_NF_TABLES_BRIDGE=n \
	CONFIG_NF_CONNTRACK=y \
	CONFIG_IP6_NF_IPTABLES=y \
	$(addsuffix =y,$(KCONFIG_IPT_ANDROID))
  FILES:=
  AUTOLOAD:=
  DEFAULT:=y
  DEPENDS:=@TARGET_realtek @kmod-android
endef

define KernelPackage/ipt-android/description
 Other Netfilter kernel builtin options for Android
endef

$(eval $(call KernelPackage,ipt-android))

define KernelPackage/systemd
  SUBMENU:=$(RTK_MENU)
  TITLE:=Support Systemd
  DEFAULT:=n
  KCONFIG:= \
	CONFIG_DEVTMPFS=y \
	CONFIG_CGROUPS=y \
	CONFIG_INOTIFY_USER=y \
	CONFIG_SIGNALFD=y \
	CONFIG_TIMERFD=y \
	CONFIG_EPOLL=y \
	CONFIG_NET=y \
	CONFIG_SYSFS=y \
	CONFIG_PROC_FS=y \
	CONFIG_SYSFS_DEPRECATED =y \
	CONFIG_UEVENT_HELPER_PATH="" \
 	CONFIG_FW_LOADER_USER_HELPER=n \
	CONFIG_DMIID=y \
	CONFIG_BLK_DEV_BSG=y
endef

define KernelPackage/systemd/description
 Kernel config for systemd System and Service Manager
endef

$(eval $(call KernelPackage,systemd))

define KernelPackage/openmax
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=$(RTK_MENU)
  TITLE:=OpenMAX kernel options
  KCONFIG:= \
	CONFIG_ION=y \
	CONFIG_ION_RTK=y \
	CONFIG_RTK_RPC=y \
	CONFIG_ION_TEST=n \
	CONFIG_ION_DUMMY=n \
	CONFIG_ION_RTK_PHOENIX=y \
	CONFIG_FIQ_DEBUGGER=n \
	CONFIG_FSL_MC_BUS=n \
	CONFIG_RTK_CODEC=y \
	CONFIG_RTK_RESERVE_MEMORY=y \
	CONFIG_VE1_CODEC=y \
	CONFIG_VE3_CODEC=y \
	CONFIG_IMAGE_CODEC=y \
	CONFIG_ANDROID=y \
	CONFIG_ANDROID_LOW_MEMORY_KILLER_AUTODETECT_OOM_ADJ_VALUES=y \
	CONFIG_ZSMALLOC=y \
	CONFIG_REGMAP=y \
	CONFIG_REGMAP_I2C=y \
	CONFIG_JUMP_LABEL=y \
	CONFIG_KSM=y \
	CONFIG_UIO=y \
	CONFIG_UIO_ASSIGN_MINOR=y \
	CONFIG_UIO_RTK_RBUS=y \
	CONFIG_UIO_RTK_REFCLK=y \
	CONFIG_UIO_RTK_SE=y \
	CONFIG_UIO_RTK_MD=y \
	CONFIG_STAGING=y \
	CONFIG_FSL_MC_BUS=n \
	CONFIG_CMA=y \
	CONFIG_CMA_DEBUG=n \
	CONFIG_CMA_DEBUGFS=y \
	CONFIG_CMA_AREAS=7 \
	CONFIG_DMA_CMA=y \
	CONFIG_CMA_SIZE_MBYTES=32 \
	CONFIG_CMA_SIZE_SEL_MBYTES=y \
	CONFIG_CMA_SIZE_SEL_PERCENTAGE=n \
	CONFIG_CMA_SIZE_SEL_MIN=n \
	CONFIG_CMA_SIZE_SEL_MAX=n \
	CONFIG_CMA_ALIGNMENT=4 \
	CONFIG_ADF=n \

  DEPENDS:=
  FILES:=
endef

define KernelPackage/openmax/description
  This package enables kernel options for OpenMAX.
endef

$(eval $(call KernelPackage,openmax))

define KernelPackage/rtk-video
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=$(RTK_MENU)
  TITLE:=rtk-video kernel options
  KCONFIG:= \
        CONFIG_MEDIA_SUPPORT=y \
        CONFIG_RTK_HDMITX=y \
        CONFIG_RTK_HDCP_1x=y \
        CONFIG_RTK_HDMIRX=n \
        CONFIG_RTK_HDCPRX_2P2=n \
        CONFIG_CEC=n \
        CONFIG_RTD1295_CEC=y \
        CONFIG_RTK_DPTX=y \
        CONFIG_FB=y \
        CONFIG_FB_RTK=y \
        CONFIG_FB_RTK_FPGA=n \
        CONFIG_FB_SIMPLE=y \
        CONFIG_ADF=y \
        CONFIG_ADF_FBDEV=n \
        CONFIG_ADF_MEMBLOCK=n \
        CONFIG_SWITCH_GPIO=n \
        CONFIG_SOUND=y \
        CONFIG_SND=y \
        CONFIG_SND_TIMER=y \
        CONFIG_SND_PCM=y \
        CONFIG_SND_HWDEP=y \
        CONFIG_SND_RAWMIDI=y \
        CONFIG_SND_COMPRESS_OFFLOAD=y \
        CONFIG_SND_JACK=y \
        CONFIG_SND_PCM_TIMER=y \
        CONFIG_SND_HRTIMER=y \
        CONFIG_SND_SUPPORT_OLD_API=y \
        CONFIG_SND_PROC_FS=y \
        CONFIG_SND_VERBOSE_PROCFS=y \
        CONFIG_SND_DRIVERS=y \
        CONFIG_SND_HDA_PREALLOC_SIZE=64 \
        CONFIG_SND_ARM=y \
        CONFIG_SND_ARMAACI=n \
        CONFIG_SND_REALTEK=y \
        CONFIG_SND_USB=y \
        CONFIG_SND_USB_AUDIO=y \
        CONFIG_SND_SOC=y \
        CONFIG_SND_SOC_COMPRESS=y

  DEPENDS:=

  FILES:=
endef

define KernelPackage/rtk-video/description
  This package enables kernel options for rtk-video.
endef

$(eval $(call KernelPackage,rtk-video))

define KernelPackage/lib-mali
  SUBMENU:=$(LIB_MENU)
  TITLE:=Mali_kbase support
  #KCONFIG:=CONFIG_MALI_KBASE
  KCONFIG:=CONFIG_MALI_MIDGARD \
	  CONFIG_MALI_MIDGARD_ENABLE_TRACE=y \
	  CONFIG_PM_DEVFREQ=y \
	  CONFIG_MALI_DEVFREQ=y \
	  CONFIG_MALI_EXPERT=y
  DEPENDS:=
  FILES:=$(LINUX_DIR)/drivers/gpu/arm/midgard/mali_kbase.ko
  AUTOLOAD:=$(call AutoProbe,mali_kbase)
endef

$(eval $(call KernelPackage,lib-mali))

define KernelPackage/mali-wayland
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=$(RTK_MENU)
  TITLE:=mali-wayland kernel options
  KCONFIG:= \
	CONFIG_INPUT_EVDEV=y \
	CONFIG_DMA_SHARED_BUFFER_USES_KDS=y \
	CONFIG_PM_DEVFREQ=y \
	CONFIG_DEVFREQ_GOV_SIMPLE_ONDEMAND=y \
	CONFIG_ADF=y \
	CONFIG_COMPAT_NETLINK_MESSAGES=y \
	CONFIG_DMA_SHARED_BUFFER=y \
	CONFIG_DRM=y \
	CONFIG_DRM_BRIDGE=y \
	CONFIG_DRM_FBDEV_EMULATION=y \
	CONFIG_DRM_GEM_CMA_HELPER=y \
	CONFIG_DRM_KMS_CMA_HELPER=y \
	CONFIG_DRM_KMS_FB_HELPER=y \
	CONFIG_DRM_KMS_HELPER=y \
	CONFIG_DRM_RTK=y \
	CONFIG_FB=y \
	CONFIG_FB_CFB_COPYAREA=y \
	CONFIG_FB_CFB_FILLRECT=y \
	CONFIG_FB_CFB_IMAGEBLIT=y \
	CONFIG_FB_CMDLINE=y \
	CONFIG_FB_RTK=y \
	CONFIG_FB_SYS_COPYAREA=y \
	CONFIG_FB_SYS_FILLRECT=y \
	CONFIG_FB_SYS_FOPS=y \
	CONFIG_FB_SYS_IMAGEBLIT=y \
	CONFIG_HDMI=y \
	CONFIG_I2C_ALGOBIT=y \
	CONFIG_MEMORY_ISOLATION=y \
	CONFIG_MIGRATION=y \
	CONFIG_PINCTRL_RTK=y \
	CONFIG_PINCTRL_RTD129X=y \
	CONFIG_RTD129X_WATCHDOG=y \
	CONFIG_SW_SYNC=y \
	CONFIG_SW_SYNC_USER=y \
	CONFIG_SYNC=y

  DEPENDS:=
  FILES:=
endef

define KernelPackage/mali-wayland/description
  This package enables kernel options for mali-wayland.
endef

$(eval $(call KernelPackage,mali-wayland))

define KernelPackage/weston
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=$(RTK_MENU)
  TITLE:=weston kernel options
  KCONFIG:=CONFIG_CONSOLE_TRANSLATIONS=y \
	CONFIG_DUMMY_CONSOLE=y \
	CONFIG_FONT_8x16=y \
	CONFIG_FONT_8x8=y \
	CONFIG_FONT_SUPPORT=y \
	CONFIG_FRAMEBUFFER_CONSOLE=y \
	CONFIG_FRAMEBUFFER_CONSOLE_DETECT_PRIMARY=y \
	CONFIG_FRAMEBUFFER_CONSOLE_ROTATION=y \
	CONFIG_HW_CONSOLE=y \
	CONFIG_SERIAL_8250_FSL=y \
	CONFIG_SERIAL_8250_PNP=y \
	CONFIG_VT=y \
	CONFIG_VT_CONSOLE=y \
	CONFIG_VT_CONSOLE_SLEEP=y \
	CONFIG_VT_HW_CONSOLE_BINDING=y \
	CONFIG_INPUT_KEYBOARD=y \
	CONFIG_INPUT_MOUSE=y \
	CONFIG_INPUT_MOUSEDEV=y \
	CONFIG_INPUT_MOUSEDEV_PSAUX=y \
	CONFIG_INPUT_MOUSEDEV_SCREEN_X=1024 \
	CONFIG_INPUT_MOUSEDEV_SCREEN_Y=768 \
	CONFIG_KEYBOARD_ATKBD=y \
	CONFIG_MOUSE_PS2=y \
	CONFIG_MOUSE_PS2_ALPS=y \
	CONFIG_MOUSE_PS2_CYPRESS=y \
	CONFIG_MOUSE_PS2_FOCALTECH=y \
	CONFIG_MOUSE_PS2_LOGIPS2PP=y \
	CONFIG_MOUSE_PS2_SYNAPTICS=y \
	CONFIG_MOUSE_PS2_TRACKPOINT=y \
	CONFIG_SERIO=y \
	CONFIG_SERIO_SERPORT=y \
	CONFIG_HID=y \
	CONFIG_HID_GENERIC=y \
	CONFIG_USB_HID=y
  DEPENDS:=
  FILES:=
endef

define KernelPackage/weston/description
  This package enables kernel options for weston.
endef

$(eval $(call KernelPackage,weston))

define KernelPackage/rtd1295hwnat
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=Realtek 1295 HWNAT driver
  KCONFIG:= \
	CONFIG_BRIDGE=y \
	CONFIG_NET_SCHED=y \
	CONFIG_RTD_1295_HWNAT=y \
	CONFIG_BRIDGE_IGMP_SNOOPING=y \
	CONFIG_RTD_1295_MAC0_SGMII_LINK_MON=y \
	CONFIG_RTL_HARDWARE_NAT=y \
	CONFIG_RTL_819X=y \
	CONFIG_RTL_HW_NAPT=y \
	CONFIG_RTL_LAYERED_ASIC_DRIVER=y \
	CONFIG_RTL_LAYERED_ASIC_DRIVER_L3=y \
	CONFIG_RTL_LAYERED_ASIC_DRIVER_L4=y \
	CONFIG_RTL_LAYERED_DRIVER_ACL=y \
	CONFIG_RTL_LAYERED_DRIVER_L2=y \
	CONFIG_RTL_LAYERED_DRIVER_L3=y \
	CONFIG_RTL_LAYERED_DRIVER_L4=y \
	CONFIG_RTL_LINKCHG_PROCESS=y \
	CONFIG_RTL_NETIF_MAPPING=y \
	CONFIG_RTL_PROC_DEBUG=y \
	CONFIG_RTL_FASTPATH_HWNAT_SUPPORT_KERNEL_3_X=y \
	CONFIG_RTL_LOG_DEBUG=n \
	CONFIG_RTL865X_ROMEPERF=n \
	CONFIG_RTK_VLAN_SUPPORT=n \
	CONFIG_RTL_EEE_DISABLED=n \
	CONFIG_RTL_SOCK_DEBUG=n \
	CONFIG_RTL_EXCHANGE_PORTMASK=n \
	CONFIG_RTL_INBAND_CTL_ACL=n \
	CONFIG_RTL_ETH_802DOT1X_SUPPORT=n \
	CONFIG_RTL_MULTI_LAN_DEV=y \
	CONFIG_AUTO_DHCP_CHECK=n \
	CONFIG_RTL_HW_MULTICAST_ONLY=n \
	CONFIG_RTL_HW_L2_ONLY=n \
	CONFIG_RTL_MULTIPLE_WAN=n \
	CONFIG_RTL865X_LANPORT_RESTRICTION=n \
	CONFIG_RTL_IVL_SUPPORT=y \
	CONFIG_RTL_LOCAL_PUBLIC=n \
	CONFIG_RTL_HW_DSLITE_SUPPORT=n \
	CONFIG_RTL_HW_6RD_SUPPORT=n \
	CONFIG_RTL_IPTABLES_RULE_2_ACL=n \
	CONFIG_RTL_FAST_FILTER=n \
	CONFIG_RTL_ETH_PRIV_SKB=n \
	CONFIG_RTL_EXT_PORT_SUPPORT=n \
	CONFIG_RTL_HARDWARE_IPV6_SUPPORT=n \
	CONFIG_RTL_ROMEPERF_24K=n \
	CONFIG_RTL_VLAN_PASSTHROUGH_SUPPORT=n \
	CONFIG_RTL_8211F_SUPPORT=n \
	CONFIG_RTL_8367R_SUPPORT=n \
	CONFIG_RTL_HW_QOS_SUPPORT=n
  FILES:=
  AUTOLOAD:=
  DEPENDS:=@TARGET_realtek_rtd129x 
endef

define KernelPackage/rtd1295hwnat/description
  This package contains the Realtek HW NAT Driver
endef

define KernelPackage/rtd1295hwnat/config
  if PACKAGE_kmod-rtd1295hwnat

	config KERNEL_NF_CONNTRACK
		bool
		default y

	config KERNEL_IP_NF_IPTABLES
		bool
		default n

	config KERNEL_VLAN_8021Q
		bool
		default y

	config KERNEL_RTL_IVL_SUPPORT
		bool
		default n

	config KERNEL_PPP
		bool
		default n

	config KERNEL_RTL_FAST_PPPOE
		bool
		default n

	config KERNEL_RTL_8021Q_VLAN_SUPPORT_SRC_TAG
		bool
		default n

	config KERNEL_RTL_HW_QOS_SUPPORT
		bool "Enable HW QoS support"
		select KERNEL_IP_NF_IPTABLES
		default n
		help
		  Enable HW QoS for HW NAT.

	config KERNEL_RTL_VLAN_8021Q
		bool "Enable HW VLAN support"
		select KERNEL_VLAN_8021Q
		select KERNEL_RTL_IVL_SUPPORT
		default y
		help
		  Enable HW QoS for HW NAT.

	config KERNEL_RTL_TSO
		bool "Enable HW TSO support"
		default y
		depends on !KERNEL_RTL_IPTABLES_FAST_PATH
		help
		  Enable HW TSO for HW NAT.

	config KERNEL_RTL_IPTABLES_FAST_PATH
		bool "Enable fastpath support"
		select KERNEL_NF_CONNTRACK
		select KERNEL_IP_NF_IPTABLES
		select KERNEL_PPP
		select KERNEL_RTL_FAST_PPPOE
		default n
		help
		  Enable fastpath when packets go through CPU.

	config KERNEL_RTL_WAN_MAC5
		bool "Use VLAN 100 of MAC5 as WAN port"
		select KERNEL_VLAN_8021Q
		select KERNEL_RTL_VLAN_8021Q
		default n
		help
		  Disable original WAN (MAC4) port, and use MAC5 as WAN port.
		  WAN (MAC5): eth0.100
		  LAN (MAC5): eth0.200

	config KERNEL_RTL_836X_SUPPORT
		bool "Enable RTL836X series switches support"
		default n
		help
		  Support Realtek RTL8363, RTL8367, RTL8370 series switches.

	config KERNEL_RTL_JUMBO_FRAME
		bool "Enable JUMBO frame support"
		default n
		help
		  Support Realtek RTL8363, RTL8367, RTL8370 series switches.

	config KERNEL_RTL_BR_SHORTCUT
		bool "Enable bridge shortcut"
		depends on RTL8192CD
		default n
		help
		  Enable Bridge Shortcut between WiFi and HW NAT
  endif
endef

$(eval $(call KernelPackage,rtd1295hwnat))


