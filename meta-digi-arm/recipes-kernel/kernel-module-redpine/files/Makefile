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

FIRMWARE := $(addprefix $(SRC)/RS.GENR.LNX.SD_GPL/OSD/LINUX/release/, instructionSet tadm taim)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install
	# Install firmware
	mkdir -p $(INSTALL_MOD_PATH)/lib/firmware/redpine
	install -m 0644 $(FIRMWARE) $(INSTALL_MOD_PATH)/lib/firmware/redpine/
