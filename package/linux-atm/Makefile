# 
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=linux-atm
PKG_VERSION:=2.5.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).orig.tar.gz
PKG_SOURCE_URL:=http://ftp.debian.org/debian/pool/main/l/linux-atm/
PKG_MD5SUM:=9560b0e1f410a05b849dfdab465dd758

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/linux-atm
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Linux ATM library
  URL:=http://linux-atm.sourceforge.net/
endef

define Package/linux-atm/description
  This package contains a library for accessing the Linux ATM subsystem.
endef

define Package/atm-tools
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=linux-atm
  TITLE:=Linux ATM tools
  URL:=http://linux-atm.sourceforge.net/
endef

define Package/atm-tools/description
  This package contains the Linux ATM tools.
endef

define Build/Configure
	$(call Build/Configure/Default)
	# prevent autoheader invocation
	touch $(PKG_BUILD_DIR)/stamp-h.in
endef

TARGET_CFLAGS += -I$(LINUX_DIR)/include -I$(LINUX_DIR)/arch/$(LINUX_KARCH)/include

define Build/Compile
	# src/qgen is built with HOSTCC, which does not really like our LDFLAGS
	$(MAKE) -C $(PKG_BUILD_DIR)/src/qgen \
		LDFLAGS="" \
		all
	$(MAKE) -C $(PKG_BUILD_DIR) all
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/include \
		$(PKG_INSTALL_DIR)/usr/lib \
		$(1)/usr/
endef

define Package/linux-atm/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libatm.so* $(1)/usr/lib/
endef

define Package/atm-tools/install
	$(INSTALL_DIR) $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/arpd/atmarp{,d} $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/lib/network
	$(INSTALL_BIN) ./files/ipoa.sh $(1)/lib/network/
	$(INSTALL_DIR) $(1)/usr/bin/

#
#The following is disabled by default but still useful for some debugging
#
#	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/atmaddr $(1)/usr/sbin/
#	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/atmloop $(1)/usr/sbin/
#	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/atmtcp     $(1)/usr/sbin/
#	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/esi $(1)/usr/sbin/
#	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/aread $(1)/usr/bin/
#	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/awrite $(1)/usr/bin/
#	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/atmdiag $(1)/usr/bin/
#	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/atmdump $(1)/usr/bin/
#
# The following is really for some specific application, and obsolete for most of it
#
#	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/atmsigd $(1)/usr/sbin/
#	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/bus $(1)/usr/sbin/
#	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/ilmid $(1)/usr/sbin/
#	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/ilmidiag $(1)/usr/sbin/
#	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/lecs $(1)/usr/sbin/
#	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/les $(1)/usr/sbin/
#	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/mpcd $(1)/usr/sbin/
#	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/zeppelin $(1)/usr/sbin/
#	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/atmswitch $(1)/usr/bin/
#	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/saaldump $(1)/usr/bin/
#	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/sonetdiag $(1)/usr/bin/
#	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/svc_recv $(1)/usr/bin/
#	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/svc_send $(1)/usr/bin/
#	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/ttcp_atm $(1)/usr/bin/
endef

$(eval $(call BuildPackage,linux-atm))
$(eval $(call BuildPackage,atm-tools))
