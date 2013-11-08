ATH_DRV_BASEDIR := compat-wireless
ATH_FIR_BASEDIR := Firmware_Package/target/AR6003/hw2.1.1

ifneq ($(KERNELRELEASE),)

ATH_DEFINES += \
	-DCOMPAT_BASE_TREE="\"$(shell cat $(src)/$(ATH_DRV_BASEDIR)/compat_base_tree)\"" \
	-DCOMPAT_BASE_TREE_VERSION="\"$(shell cat $(src)/$(ATH_DRV_BASEDIR)/compat_base_tree_version)\"" \
	-DCOMPAT_PROJECT="\"Compat-wireless\"" \
	-DCOMPAT_VERSION="\"$(shell cat $(src)/$(ATH_DRV_BASEDIR)/compat_version)\""

NOSTDINC_FLAGS := -I$(M)/$(ATH_DRV_BASEDIR)/include/ \
	-include $(M)/$(ATH_DRV_BASEDIR)/include/linux/compat-2.6.h \
	$(ATH_DEFINES)

include $(src)/$(ATH_DRV_BASEDIR)/config.mk

SHELL_EXPORT := PATH=$(src)/$(ATH_DRV_BASEDIR)/scripts:$${PATH} \
		COMPAT_CONFIG=$(src)/$(ATH_DRV_BASEDIR)/config.mk \
		CONFIG_CHECK=.$(COMPAT_CONFIG)_md5sum.txt \
		COMPAT_AUTOCONF=$(src)/$(ATH_DRV_BASEDIR)/include/linux/compat_autoconf.h

dummy := $(shell $(SHELL_EXPORT) bash -c "cd $(src)/$(ATH_DRV_BASEDIR) && ./scripts/check_config.sh || true")

obj-y := $(ATH_DRV_BASEDIR)/compat/
obj-y += $(ATH_DRV_BASEDIR)/net/wireless/
obj-y += $(ATH_DRV_BASEDIR)/drivers/net/wireless/ath/ath6kl/

else #ifneq ($(KERNELRELEASE),)

## Firmware files
FIRMWARE := $(ATH_FIR_BASEDIR)/fw_tablet_dongle/athtcmd_ram.bin
FIRMWARE += $(ATH_FIR_BASEDIR)/fw_tablet_dongle/athwlan.bin
FIRMWARE += $(ATH_FIR_BASEDIR)/fw_tablet_dongle/fw-4.bin
FIRMWARE += $(ATH_FIR_BASEDIR)/fw_tablet_dongle/nullTestFlow.bin
FIRMWARE += $(ATH_FIR_BASEDIR)/fw_tablet_dongle/utf.bin
FIRMWARE += $(ATH_FIR_BASEDIR)/Digi_6203-6233-US.bin
FIRMWARE += $(ATH_FIR_BASEDIR)/Digi_6203-6233-World.bin

SRC := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install
	# Fix installation directory of the modules.
	find $(INSTALL_MOD_PATH)/lib/modules/$(KERNEL_VERSION)/extra/$(ATH_DRV_BASEDIR) -type f -name '*.ko' | \
		xargs -I modfile mv -f modfile $(INSTALL_MOD_PATH)/lib/modules/$(KERNEL_VERSION)/extra/
	rm -rf $(INSTALL_MOD_PATH)/lib/modules/*/extra/$(ATH_DRV_BASEDIR)
	# Install firmware
	mkdir -p $(INSTALL_MOD_PATH)/lib/firmware/ath6k/AR6003/hw2.1.1/
	install -m 0644 $(FIRMWARE) $(INSTALL_MOD_PATH)/lib/firmware/ath6k/AR6003/hw2.1.1/

endif #ifneq ($(KERNELRELEASE),)
