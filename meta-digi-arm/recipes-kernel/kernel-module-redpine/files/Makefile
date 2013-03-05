## NON_GPL module (rsi_client.ko)
CLIENT_OBJS += RS.GENR.LNX.SD_NON_GPL/OSD/LINUX/client/ganges_client_init.o
CLIENT_OBJS += RS.GENR.LNX.SD_NON_GPL/WLAN/OSI/ganges_802.11d.o
CLIENT_OBJS += RS.GENR.LNX.SD_NON_GPL/WLAN/OSI/ganges_802.11h.o
CLIENT_OBJS += RS.GENR.LNX.SD_NON_GPL/WLAN/OSI/ganges_fsm.o
CLIENT_OBJS += RS.GENR.LNX.SD_NON_GPL/WLAN/OSI/ganges_ioctl.o
CLIENT_OBJS += RS.GENR.LNX.SD_NON_GPL/WLAN/OSI/ganges_mgmt.o
CLIENT_OBJS += RS.GENR.LNX.SD_NON_GPL/WLAN/OSI/ganges_osi_data.o
CLIENT_OBJS += RS.GENR.LNX.SD_NON_GPL/WLAN/OSI/ganges_osi_dev_ops.o
CLIENT_OBJS += RS.GENR.LNX.SD_NON_GPL/WLAN/OSI/ganges_power.o
CLIENT_OBJS += RS.GENR.LNX.SD_NON_GPL/WLAN/OSI/ganges_generic.o
CLIENT_OBJS += RS.GENR.LNX.SD_NON_GPL/WLAN/WLAN_OS/LINUX/ganges_linux_functional.o
CLIENT_OBJS += RS.GENR.LNX.SD_NON_GPL/HAL/OSI/SDIO/ganges_sdio_specific.o

## GPL module (rsi_master.ko)
MASTER_OBJS += RS.GENR.LNX.SD_GPL/OSD/LINUX/master/ganges_linux_main.o
MASTER_OBJS += RS.GENR.LNX.SD_GPL/OSD/LINUX/master/ganges_master_init.o
MASTER_OBJS += RS.GENR.LNX.SD_GPL/OSD/LINUX/master/ganges_linux_master_ops.o
MASTER_OBJS += RS.GENR.LNX.SD_GPL/OSD/LINUX/master/ganges_linux_specific.o
MASTER_OBJS += RS.GENR.LNX.SD_GPL/OSD/LINUX/master/ganges_linux_osd_data.o
MASTER_OBJS += RS.GENR.LNX.SD_GPL/OSD/LINUX/master/ganges_parser.o
MASTER_OBJS += RS.GENR.LNX.SD_GPL/OSD/LINUX/master/ganges_linux_ioctl.o
MASTER_OBJS += RS.GENR.LNX.SD_GPL/HAL/SDIO/LINUX/master/ganges_linux_sdio.o
MASTER_OBJS += RS.GENR.LNX.SD_GPL/OSD/LINUX/master/digi_hal.o

## Firmware files
FIRMWARE := RS.GENR.LNX.SD_GPL/OSD/LINUX/release/instructionSet
FIRMWARE += RS.GENR.LNX.SD_GPL/OSD/LINUX/release/tadm
FIRMWARE += RS.GENR.LNX.SD_GPL/OSD/LINUX/release/taim

ifneq ($(KERNELRELEASE),)

EXTRA_CFLAGS += -DLINUX -DUSE_SDIO_INTF

obj-m := rsi_client.o rsi_master.o
rsi_client-y := $(CLIENT_OBJS)
rsi_master-y := $(MASTER_OBJS)

else #ifneq ($(KERNELRELEASE),)

SRC := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install
	# Install firmware
	mkdir -p $(INSTALL_MOD_PATH)/lib/firmware/redpine
	install -m 0644 $(FIRMWARE) $(INSTALL_MOD_PATH)/lib/firmware/redpine/

tarball: TAR_DIR = $(SRC)/_tarball
tarball:
	for i in $(CLIENT_OBJS); do \
		mkdir -p $(TAR_DIR)/$$(dirname $${i}); \
		install -m 0644 $${i} $(TAR_DIR)/$$(dirname $${i})/$$(basename $${i})_shipped; \
	done
	for i in $(MASTER_OBJS:.o=.c) $(FIRMWARE); do \
		mkdir -p $(TAR_DIR)/$$(dirname $${i}); \
		install -m 0644 $${i} $(TAR_DIR)/$$(dirname $${i})/; \
	done
	cp -r RS.GENR.LNX.SD_GPL/include $(TAR_DIR)/RS.GENR.LNX.SD_GPL/
	rm -f redpine-$(DEL_PLATFORM).tar.gz && tar cz --transform 's,\(^[^/]\+/\),\1$(DEL_PLATFORM)/,' \
		--numeric-owner --owner 0 --group 0 -f redpine-$(DEL_PLATFORM).tar.gz -C $(TAR_DIR) .
	-rm -rf $(TAR_DIR)

endif #ifneq ($(KERNELRELEASE),)
