#!/bin/sh

# The bit settings of drv_mode are:
#    Bit 0 :  STA
#    Bit 1 :  uAP
#    Bit 2 :  WIFIDIRECT
# eg, for STA + uAP + WIFIDIRECT, set 7 (b'111)
DRIVER_MODE=1  # Only STA

# MREG_D(00000200), MERROR(00000004),MFATAL(00000002)
DRIVER_DEBUG="0x206"

MOAL_PARAMS=" \
cfg80211_wext=0xf \
max_vir_bss=1 \
cal_data_cfg=none \
ps_mode=1 \
auto_ds=1 \
host_mlme=1 \
drv_mode=${DRIVER_MODE} \
drvdbg=${DRIVER_DEBUG} \
sta_name=wlan \
fw_name=nxp/sduart_nw61x_v1.bin.se\
"

log() {
        printf "<3>iw612-wifi: $1\n" >/dev/kmsg
}

if ! [ -e "/proc/device-tree/wireless/mac-address" ]; then
	log "[ERROR] wireless mac-address not found"
	exit 1
fi

WLANADDR=$(hexdump -ve '1/1 "%02X" ":"' /proc/device-tree/wireless/mac-address 2>/dev/null | sed 's/:$//g')
modprobe mlan && \
modprobe moal ${MOAL_PARAMS} mac_addr=${WLANADDR} && \
log "Wi-Fi activated" && exit 0

log "[ERROR] cannot load Wi-Fi driver"
exit 1
