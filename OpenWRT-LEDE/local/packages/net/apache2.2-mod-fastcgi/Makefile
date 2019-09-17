#
# Copyright (C) 2007-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mod_fastcgi
PKG_VERSION:=9bd2a9a
PKG_FILENAME:=9bd2a9afcf0196a0314af9d6b3def759a23018ad
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_FILENAME).tar.gz
PKG_SOURCE_URL:= https://repo.or.cz/mod_fastcgi.git/snapshot/
PKG_MD5SUM:=2212c3e5c9f540aaca0a50e3f52a8100
PKG_LICENSE:=Open Market, Inc

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/apache2.2-mod-fastcgi
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Web Servers/Proxies
  DEPENDS:=+libpthread +librt +libuuid +apache2.2 +php5 +php5-fpm +php5-mod-opcache
  TITLE:=Apache Fast CGI module
  URL:=https://fastcgi-archives.github.io/mod_fastcgi.html
endef

TARGET_CFLAGS += $(FPIC)
TARGET_CPPFLAGS += -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE

MAKE_FLAGS = \
	top_dir="$(STAGING_DIR)/apache"

define Build/Compile
	$(CP)	$(PKG_BUILD_DIR)/Makefile.AP2	$(PKG_BUILD_DIR)/Makefile
	$(call Build/Configure/Default, )
endef

define Package/apache2.2-mod-fastcgi/install
	$(INSTALL_DIR) $(1)/usr/lib

	# module link library, same to Package/apache/install in apache Makefile
	$(INSTALL_DIR) $(1)/usr/lib/apache
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.so $(1)/usr/lib/apache

	# update config files
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) ./files/php.ini $(1)/etc
	$(INSTALL_DIR) $(1)/etc/apache
	$(INSTALL_DATA) ./files/httpd.conf $(1)/etc/apache
	$(INSTALL_DIR) $(1)/etc/php5-fpm.d
	$(INSTALL_DATA) ./files/www.conf $(1)/etc/php5-fpm.d
endef

$(eval $(call BuildPackage,apache2.2-mod-fastcgi))
