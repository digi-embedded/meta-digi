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

EXTRA_CFLAGS += -DLINUX -DUSE_SDIO_INTF

obj-m := rsi_client.o rsi_master.o
rsi_client-y := $(CLIENT_OBJS)
rsi_master-y := $(MASTER_OBJS)

SRC := $(shell pwd)

SHELL     = /bin/bash
FIRMWARE := $(addprefix $(SRC)/RS.GENR.LNX.SD_GPL/OSD/LINUX/release/, instructionSet tadm taim)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install
	# Install firmware
	mkdir -p $(INSTALL_MOD_PATH)/lib/firmware/redpine
	install -m 0644 $(FIRMWARE) $(INSTALL_MOD_PATH)/lib/firmware/redpine/

tarball: TAR_DIR = $(SRC)/_tarball
tarball:
	mkdir -p $(TAR_DIR)/{firmware,GPL/{HAL/SDIO/LINUX/master,OSD/LINUX/master},NON_GPL}
	install -m 0644 $(MASTER_OBJS:.o=.c) $(TAR_DIR)/GPL/OSD/LINUX/master/
	mv $(TAR_DIR)/GPL/OSD/LINUX/master/ganges_linux_sdio.c $(TAR_DIR)/GPL/HAL/SDIO/LINUX/master/
	cp -r RS.GENR.LNX.SD_GPL/include $(TAR_DIR)/GPL/
	for i in $(CLIENT_OBJS); do \
		install -m 0644 $${i} $(TAR_DIR)/NON_GPL/$$(basename $${i})_shipped; \
	done
	install -m 0644 $(FIRMWARE) $(TAR_DIR)/firmware/
	rm -f redpine-$(DEL_PLATFORM).tar.gz && tar cz --transform 's,\(^[^/]\+/\),\1$(DEL_PLATFORM)/,' \
		--numeric-owner --owner 0 --group 0 -f redpine-$(DEL_PLATFORM).tar.gz -C $(TAR_DIR) .
	-rm -rf $(TAR_DIR)
