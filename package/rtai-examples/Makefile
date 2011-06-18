#
# Copyright (C) 2011 Bifferos.com (sales@bifferos.com)
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk


PKG_NAME:=rtai-led-blink
PKG_RELEASE:=1
PKG_BUILD_DEPENDS:=liblxrt libpthread

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/rtai-led-blink
  SECTION:=utils
  CATEGORY:=Base system
  TITLE:=RTAI led blink example
  DEPENDS:=+liblxrt +libpthread
endef

define Package/rtai-led-blink/description
  Uses RTAI to blink the red LED on and off.
  Remove the watchdog module before running this example.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(shell $(STAGING_DIR)/usr/bin/rtai-config --cc)" \
		LXRT_CFLAGS="-I$(STAGING_DIR)/usr/include -Wall -Wstrict-prototypes -pipe" \
		LXRT_LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib -lpthread"
endef

define Package/rtai-led-blink/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rtai-led-blink $(1)/usr/bin
endef

$(eval $(call BuildPackage,rtai-led-blink))
