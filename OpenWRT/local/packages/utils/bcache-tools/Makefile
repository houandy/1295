include $(TOPDIR)/rules.mk

PKG_NAME:=bcache-tools
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_VERSION:=v1.0.8
PKG_SOURCE_VERSION:=a73679b22c333763597d39c72112ef5a53f55419
PKG_SOURCE_URL:=https://evilpiepirate.org/git/bcache-tools.git

include $(INCLUDE_DIR)/package.mk

define Package/bcache-tools
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=Disc
  DEPENDS:=+kmod-bcache +libopenssl +libblkid +libuuid
  TITLE:=Linux kernel block layer cache
  URL:=https://bcache.evilpiepirate.org
endef

define Package/bcache/description
    Bcache is a Linux kernel block layer cache. It allows one
    or more fast disk drives such as flash-based solid state
    drives (SSDs) to act as a cache for one or more slower
    hard disk drives
endef

define Package/bcache-tools/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/make-bcache $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/probe-bcache $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bcache-super-show $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bcache-register $(1)/usr/bin/
endef

$(eval $(call BuildPackage,bcache-tools))
