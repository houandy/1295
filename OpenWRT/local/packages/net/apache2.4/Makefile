#
# Copyright (C) 2007-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=apache
PKG_VERSION:=2.4.28
PKG_RELEASE:=2
PKG_SOURCE_NAME:=httpd
PKG_MAINTAINER:=Thomas Heil <heil@terminal-consulting.de>
PKG_LICENSE:=Apache License

PKG_SOURCE:=$(PKG_SOURCE_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@APACHE/httpd/
PKG_HASH:=c1197a3a62a4ab5c584ab89b249af38cf28b4adee9c0106b62999fd29f920666

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_NAME)-$(PKG_VERSION)

PKG_INSTALL:=1

PKG_CONFIG_DEPENDS := \
	CONFIG_APACHE_HTTP2

ADDITIONAL_MODULES:=
ifeq ($(CONFIG_APACHE_HTTP2),y)
  ADDITIONAL_MODULES += --enable-http2
endif
ifneq ($(CONFIG_APACHE_HTTP2),y)
  ADDITIONAL_MODULES += --enable-http2=no
endif

include $(INCLUDE_DIR)/package.mk

define Package/apache2.4/Default
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Web Servers/Proxies
  TITLE:=The Apache Web Server
  URL:=http://httpd.apache.org/
endef

define Package/apache2.4/Default/description
 The Apache Web Server is a powerful and flexible HTTP/1.1 compliant
 web server.  Originally designed as a replacement for the NCSA HTTP
 Server, it has grown to be the most popular web server on the Internet.
endef

define Package/apache2.4/config
  source "$(SOURCE)/Config.in"
endef

define Package/apache2.4
$(call Package/apache2.4/Default)
  DEPENDS:=+libapr +libaprutil +libpcre +libopenssl +unixodbc +zlib +APACHE_HTTP2:libnghttp2
endef

define Package/apache2.4/description
$(call Package/apache2.4/Default/description)
 .
 This package contains the Apache web server and utility programs.
 .
 Take care that you don't include apache at the moment into your image
 please select it only as module because busybox will override
 /usr/sbin/httpd. It'll be solved soon. If you need to include this
 package in the image anyway, remove httpd from busybox
 (Base system --> Configuration --> Networking Utilities --> httpd).
 Also you should take care for the initscripts, apache's httpd isn't
 compatible with the one from busybox, so if you want to use apache
 for running your webif, you'll need to change the parameters in the
 scripts and configure the rest in /etc/httpd.conf.
endef

define Package/apache2.4/conffiles
/etc/apache/httpd.conf
/etc/apache/extra/httpd-autoindex.conf
/etc/apache/extra/httpd-dav.conf
/etc/apache/extra/httpd-default.conf
/etc/apache/extra/httpd-info.conf
/etc/apache/extra/httpd-languages.conf
/etc/apache/extra/httpd-manual.conf
/etc/apache/extra/httpd-mpm.conf
/etc/apache/extra/httpd-multilang-errordoc.conf
/etc/apache/extra/httpd-ssl.conf
/etc/apache/extra/httpd-userdir.conf
/etc/apache/extra/httpd-vhosts.conf
/etc/apache/magic
/etc/apache/mime.types
endef

define Package/apache2.4-icons
$(call Package/apache2.4/Default)
  TITLE:=Icons from Apache
  DEPENDS:=apache2.4
endef

define Package/apache2.4-icons/description
$(call Package/apache2.4/Default/description)
 .
 This package contains the icons from Apache.
endef

TARGET_CFLAGS += $(FPIC)
TARGET_CPPFLAGS += -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE
TARGET_LDFLAGS += -lpthread

CONFIGURE_ARGS+= \
		--with-apr="$(STAGING_DIR)/usr/bin/apr-1-config" \
		--with-apr-util="$(STAGING_DIR)/usr/bin/apu-1-config" \
		--with-pcre="$(STAGING_DIR)/usr/bin/pcre-config" \
		$(ADDITIONAL_MODULES) \
		--enable-http \
		--with-crypto \
		--with-sqlit3="$(STAGING_DIR)/usr" \
		--with-openssl="$(STAGING_DIR)/usr" \
		--enable-so=static \
		--enable-ssl=shared \
		--enable-proxy=shared \
		--enable-mod_access_compat=shared \
		--enable-mod_unixd=shared \
		--disable-disk-cache \
		--enable-maintainer-mode=shared \
		--with-mpm=prefork \
		--with-mpm=worker \
		--enable-mime=shared \
		--enable-mime-magic=shared \
		--without-suexec-bin \
		--sysconfdir=/etc/apache \
		ap_cv_void_ptr_lt_long=no \
		logfiledir="/var/log" \
		runtimedir="/var/run" \
		EXTRA_LIBS="-ldl -lpthread -lcrypto -lrt -lssl" \

ifeq ($(CONFIG_PACKAGE_apache2.4-mod-fastcgi), y)
CONFIGURE_ARGS+= \
		--enable-actions=shared \
		--enable-alias=shared \
		--enable-authz-host=shared \
		--enable-authz-core=shared \
		--enable-authz-user=shared \
		--enable-mime-magic=shared

#		--enable-asis=shared \
		--enable-auth-basic=shared \
		--enable-authn-default=shared \
		--enable-authn-file=shared \
		--enable-authz-default=shared \
		--enable-authz-groupfile=shared \
		--enable-autoindex=shared \
		--enable-cgid=shared \
		--enable-dir=shared \
		--enable-env=shared \
		--enable-filter=shared \
		--enable-include=shared \
		--enable-log-config=shared \
		--enable-negotiation=shared \
		--enable-setenvif=shared \
		--enable-status=shared \
		--enable-userdir=shared \
		--enable-version=shared \

endif

define Build/Configure
	$(call Build/Configure/Default)

	# for some modules of apache to compile
	$(INSTALL_DIR) $(STAGING_DIR)/apache
	$(CP) $(PKG_BUILD_DIR)/* $(STAGING_DIR)/apache
endef

define Build/InstallDev
	rm -rf	$(PKG_INSTALL_DIR)/usr/man/ \
		$(PKG_INSTALL_DIR)/usr/share/manual/
	# if you need docs take a look into the build-dir :)
	$(INSTALL_DIR) $(1)/etc
	$(CP)   $(PKG_INSTALL_DIR)/etc/* \
		$(1)/etc
	$(INSTALL_DIR) $(1)/usr/include/apache
	$(CP)	$(PKG_INSTALL_DIR)/usr/include/* \
		$(1)/usr/include/apache
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP)   $(PKG_INSTALL_DIR)/usr/lib/httpd.exp \
		$(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP)   $(PKG_INSTALL_DIR)/usr/sbin/* \
		$(1)/usr/sbin
	$(INSTALL_DIR) $(1)/usr/share
	$(CP)   $(PKG_INSTALL_DIR)/usr/share/* \
		$(1)/usr/share
endef

define Package/apache2.4/preinst
	rm /usr/sbin/httpd
	echo -e "You should take a look in the initscripts, busybox's httpd \n\
	uses some parameters which are maybe unsupported by apache."
endef

define Package/apache2.4/install
	$(INSTALL_DIR) $(1)/usr/sbin
	# we don't need apxs on the router, it's just for building apache modules.
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/{ab,dbmmanage,htdbm,htdigest,htpasswd,httxt2dbm,logresolve} $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/{checkgid,envvars,envvars-std,htcacheclean,httpd,rotatelogs} $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/apachectl $(1)/usr/sbin/apachectl
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/httpd.exp $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/share
	$(CP) $(PKG_INSTALL_DIR)/usr/share/{error,htdocs,cgi-bin,build} $(1)/usr/share/
	$(INSTALL_DIR) $(1)/etc/apache

    ifeq ($(CONFIG_PACKAGE_apache2.4-mod-fastcgi), y)
	$(CP) $(PKG_INSTALL_DIR)/etc/apache/{magic,mime.types,extra} $(1)/etc/apache/
    else
	$(CP) $(PKG_INSTALL_DIR)/etc/apache/{httpd.conf,magic,mime.types,extra} $(1)/etc/apache/
    endif

	# module link library
	$(INSTALL_DIR) $(1)/usr/lib/apache
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.so $(1)/usr/lib/apache
endef

define Package/apache2.4/postrm
	rm -rf /usr/sbin/httpd
	ln -s /bin/busybox /usr/sbin/httpd
	echo -e "You may need to change your initscripts back for the use \n\
		with busybox's httpd."
endef

define Package/apache2.4-icons/install
	$(INSTALL_DIR) $(1)/usr/share
	$(CP) $(PKG_INSTALL_DIR)/usr/share/icons $(1)/usr/share/
endef

$(eval $(call BuildPackage,apache2.4))
$(eval $(call BuildPackage,apache2.4-icons))
