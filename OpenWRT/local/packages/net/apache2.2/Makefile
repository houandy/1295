#
# Copyright (C) 2007-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=apache
PKG_VERSION:=2.2.31
PKG_RELEASE:=1
PKG_SOURCE_NAME:=httpd
PKG_MAINTAINER:=Thomas Heil <heil@terminal-consulting.de>
PKG_LICENSE:=Apache License

PKG_SOURCE:=$(PKG_SOURCE_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://mirrors.ibiblio.org/apache/httpd/ \
		http://apache.imsam.info/httpd/
PKG_MD5SUM:=6c10e15835ab214464228a9beb7afba8

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_NAME)-$(PKG_VERSION)

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/apache2.2/Default
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Web Servers/Proxies
  TITLE:=The Apache Web Server
  URL:=http://httpd.apache.org/
endef

define Package/apache2.2/Default/description
 The Apache Web Server is a powerful and flexible HTTP/1.1 compliant
 web server.  Originally designed as a replacement for the NCSA HTTP
 Server, it has grown to be the most popular web server on the Internet.
endef

define Package/apache2.2
$(call Package/apache2.2/Default)
  DEPENDS:=+libapr +libaprutil +libpcre +libopenssl +unixodbc
endef

define Package/apache2.2/description
$(call Package/apache2.2/Default/description)
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

define Package/apache2.2/conffiles
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

define Package/apache2.2-icons
$(call Package/apache2.2/Default)
  TITLE:=Icons from Apache
  DEPENDS:=apache2.2
endef

define Package/apache2.2-icons/description
$(call Package/apache2.2/Default/description)
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
		--enable-http \
		--disable-disk-cache \
		--enable-maintainer-mode=shared \
		--enable-mime=shared \
		--enable-so=static \
		--enable-ssl=shared \
		--enable-proxy=shared \
		--with-mpm=worker \
		--without-suexec-bin \
		--sysconfdir=/etc/apache \
		ap_cv_void_ptr_lt_long=no \
		logfiledir="/var/log" \
		runtimedir="/var/run"

ifeq ($(CONFIG_PACKAGE_apache2.2-mod-fastcgi), y)
CONFIGURE_ARGS+= \
		--enable-actions=shared \
		--enable-alias=shared \
		--enable-authz-host=shared \
		--enable-mime-magic=shared

#		--enable-asis=shared \
		--enable-auth-basic=shared \
		--enable-authn-default=shared \
		--enable-authn-file=shared \
		--enable-authz-default=shared \
		--enable-authz-groupfile=shared \
		--enable-authz-user=shared \
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

	# for some modules of apache to compile, ex: mod-fastcgi
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

define Package/apache2.2/preinst
	rm /usr/sbin/httpd
	echo -e "You should take a look in the initscripts, busybox's httpd \n\
	uses some parameters which are maybe unsupported by apache."
endef

define Package/apache2.2/install
	$(INSTALL_DIR) $(1)/usr/sbin
	# we don't need apxs on the router, it's just for building apache modules.
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/{ab,checkgid,dbmmanage,envvars,envvars-std,htcacheclean,htdbm,htdigest,htpasswd,httpd,httxt2dbm,logresolve,rotatelogs} $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/apachectl $(1)/usr/sbin/apachectl
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/httpd.exp $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/share
	$(CP) $(PKG_INSTALL_DIR)/usr/share/{error,htdocs,cgi-bin,build} $(1)/usr/share/
	$(INSTALL_DIR) $(1)/etc/apache
    ifeq ($(CONFIG_PACKAGE_apache2.2-mod-fastcgi), y)
	$(CP) $(PKG_INSTALL_DIR)/etc/apache/{magic,mime.types,extra} $(1)/etc/apache/
    else
	$(CP) $(PKG_INSTALL_DIR)/etc/apache/{httpd.conf,magic,mime.types,extra} $(1)/etc/apache/
    endif

	# module link library
	$(INSTALL_DIR) $(1)/usr/lib/apache
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.so $(1)/usr/lib/apache
endef

define Package/apache2.2/postrm
	rm -rf /usr/sbin/httpd
	ln -s /bin/busybox /usr/sbin/httpd
	echo -e "You may need to change your initscripts back for the use \n\
		with busybox's httpd."
endef


define Package/apache2.2-icons/install
	$(INSTALL_DIR) $(1)/usr/share
	$(CP) $(PKG_INSTALL_DIR)/usr/share/icons $(1)/usr/share/
endef

$(eval $(call BuildPackage,apache2.2))
$(eval $(call BuildPackage,apache2.2-icons))
