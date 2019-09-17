#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=weston
PKG_VERSION:=3.0.0
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://anongit.freedesktop.org/wayland/weston
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=71c4f70e08faad6002ec8fe8cd1c7930bee8373b
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
PKG_FIXUP:=libtool
PKG_INSTALL:=1
#PKG_BUILD_DEPENDS:=xorg-macros/host
PKG_BUILD_DEPENDS:=libpthread-stubs xcbproto xproto

include $(INCLUDE_DIR)/package.mk

define Package/weston
  SECTION:=xorg-framework
  CATEGORY:=Xorg
  SUBMENU:=Weston
  TITLE:=Weston
  URL:=http://cgit.freedesktop.org/wayland/weston
  DEPENDS:=+kmod-weston +mali-wayland \
	+libdrm +pixman +libinput +libevdev \
	+libxkbcommon +libxau +eudev +wayland-protocols \
	+libjpeg +libpng +libpam +WESTON_ENABLE_DBUS:dbus +WESTON_ENABLE_WESTON_LAUNCH:libpam  \
	+cairo
endef

define Package/weston/description
  The Weston compositor is a minimal and fast compositor and is suitable for many embedded and mobile use cases.
endef

ifeq ($(CONFIG_RTK_KERNEL_4_4_18),y)

CONFIGURE_ARGS += \
	--enable-egl \
	--enable-simple-egl-clients \
	--with-cairo=image \
	--disable-x11-compositor \
	--disable-xwayland-test \
	--disable-xwayland \
	--disable-setuid-install

EXTRA_LDFLAGS+= -Wl,-rpath-link=$(STAGING_DIR)/lib

else #!CONFIG_RTK_KERNEL_4_4_18

MALI_WAYLAND_SERVER_STAGING:=$(STAGING_DIR)/usr/local/mali-wayland-server
MALI_WAYLAND_CLIENT_STAGING:=$(STAGING_DIR)/usr
#PKG_CONFIG_PATH:=$(MALI_WAYLAND_SERVER_STAGING)/lib/pkgconfig:$(PKG_CONFIG_PATH)
#TARGET_LDFLAGS:=-L$(MALI_WAYLAND_SERVER_STAGING)/lib $(TARGET_LDFLAGS)
#TARGET_CONFIGURE_OPTS = \
	EGL_LIBS="-L$(MALI_WAYLAND_SERVER_STAGING)/lib -lEGL -lGLESv2 -lmali" \
	EGL_CFLAGS="-I$(MALI_WAYLAND_SERVER_STAGING)/include" \
	EGL_TESTS_LIBS="-L$(MALI_WAYLAND_CLIENT_STAGING)/lib -lEGL -lGLESv2 -lmali" \
	EGL_TESTS_CFLAGS="-I$(MALI_WAYLAND_CLIENT_STAGING)/include"

CONFIGURE_ARGS += \
	EGL_LIBS="-L$(MALI_WAYLAND_CLIENT_STAGING)/lib -lEGL-server -lGLESv2-server -lmali-server" \
	EGL_CFLAGS="-I$(MALI_WAYLAND_SERVER_STAGING)/include" \
	--disable-silent-rules \
	LIBS="-ldrm -ludev"

CONFIGURE_ARGS += \
	--enable-egl \
	--with-cairo=image \
	--disable-x11-compositor \
	--disable-xwayland-test \
	--disable-xwayland \
	--disable-setuid-install \
	--disable-simple-egl-clients

endif

TARGET_CFLAGS += -DGL_GLEXT_PROTOTYPES -DWL_EGL_PLATFORM

define Build/Prepare
	$(call Build/Prepare/Default)
	#echo "#define BUILD_ID \"$(PKG_SOURCE_VERSION)-git ($(PKG_VERSION)) (OpenWrt build)\"" > $(PKG_BUILD_DIR)/src/git-version.h
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/bin \
		$(1)/usr/lib/weston \
		$(1)/usr/share/weston \
		$(1)/usr/include/weston
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/** $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/weston/*.h $(1)/usr/include/weston/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/weston-desktop-shell $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/weston-keyboard $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/weston-screenshooter $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/weston-simple-im $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/weston/*.so $(1)/usr/lib/weston/
	$(CP) $(PKG_INSTALL_DIR)/usr/share/weston/*.* $(1)/usr/share/weston/
endef

define Package/weston/install
	$(INSTALL_DIR) $(1)/usr/lib/weston \
		$(1)/usr/bin \
		$(1)/usr/share/weston
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/* $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/weston $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/weston-desktop-shell $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/weston-keyboard $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/weston-screenshooter $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/weston-simple-im $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/weston/*.so $(1)/usr/lib/weston/
	$(CP) $(PKG_INSTALL_DIR)/usr/share/weston/*.* $(1)/usr/share/weston/
	$(INSTALL_DIR) $(1)/etc/hotplug.d/input
	$(INSTALL_BIN) ./files/10-input-tag $(1)/etc/hotplug.d/input
	$(INSTALL_DIR) $(1)/etc/xdg/weston
	$(INSTALL_BIN) ./files/weston.ini $(1)/etc/xdg/weston
  ifeq ("$(CONFIG_RTK_KERNEL_4_9)","y")
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/weston-init.ini $(1)/etc/init.d/weston
  endif
endef

$(eval $(call BuildPackage,weston))
