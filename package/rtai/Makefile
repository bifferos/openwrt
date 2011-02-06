#
# OpenWrt Makefile for RTAI, (c) Bifferos.com (sales@bifferos.com)
#
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=rtai
PKG_VERSION:=stable_cvs_20110206
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@SF/bifferboard/

PKG_BUILD_PARALLEL:=1
PKG_INSTALL:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)_$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk


define Package/liblxrt
 SECTION:=libs
 CATEGORY:=Libraries
 TITLE:= lxrt libraries from RTAI
 URL:=http://www.rtai.org/
endef

define Package/liblxrt/description
 This package contains the RTAI lxrt library
endef


define Build/Configure
	( cd $(PKG_BUILD_DIR); \
		CROSS_COMPILE=$(TARGET_CROSS) \
		./configure \
		--with-linux-dir=$(LINUX_DIR) \
		--host=i386 \
		--prefix=$(PKG_INSTALL_DIR) \
		--disable-math \
		--disable-fpu \
		--disable-math-c99 \
		--disable-leds \
		--disable-diag-tsc-sync \
		--disable-tune-tsc-sync \
		--disable-testsuite \
		CC=$(TARGET_CROSS)gcc \
		CXX=$(TARGET_CROSS)cxx \
		LD=$(TARGET_CROSS)ld \
		AR=$(TARGET_CROSS)ar \
		RANLIB=$(TARGET_CROSS)ranlib \
		STRIP=$(TARGET_CROSS)strip \
		NM=$(TARGET_CROSS)nm \
	)
endef


define Package/liblxrt/install
	echo "TODO: installing lxrt"
endef

$(eval $(call BuildPackage,liblxrt))
