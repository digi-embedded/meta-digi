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
country_ie_ignore=1 \
txpwrlimit_cfg=nxp/txpower_US.bin \
init_hostcmd_cfg=nxp/rutxpower_US.bin \
fw_name=nxp/sd_w61x_v1.bin.se \
"

MMC_NODE="428b0000.mmc"
# Lock file to track and prevent to re-run the script when rebinding the MMC node.
LOCKFILE="/tmp/iw61x.lock"

log() {
        printf "<3>iw61x-wifi: $1\n" >/dev/kmsg
}

if test -f "$LOCKFILE"; then
	# Script called due to rebinding of mmc. Ignore it and remove the lock file.
	rm $LOCKFILE
	exit 1
fi

if ! [ -e "/proc/device-tree/wireless/mac-address" ]; then
	log "[ERROR] wireless mac-address not found"
	exit 1
fi
WLANADDR=$(hexdump -ve '1/1 "%02X" ":"' /proc/device-tree/wireless/mac-address 2>/dev/null | sed 's/:$//g')

# Force re-detection of the mmc node
rebind_mmc_node() {
	DRIVER_NODE=$(find /sys/bus/platform/drivers -name ${MMC_NODE} | xargs dirname 2> /dev/null) || return 1
	echo ${MMC_NODE} > ${DRIVER_NODE}/unbind
	# Give some time to the mmc driver to re-detect the MMC node in order to re-initialize it.
	sleep 2
	echo ${MMC_NODE} > ${DRIVER_NODE}/bind
}

load_and_check() {
    iw reg set US && \
    modprobe mlan && \
    modprobe moal ${MOAL_PARAMS} mac_addr=${WLANADDR} && \
    sleep $1 && [ -d "/sys/class/net/wlan0" ]
}

load_and_check 0 && log "Wi-Fi activated" && exit 0

# If we are here, the load has failed. Unload (unconditionally) the driver in case it was loaded and retry.
log "[WARN] Loading moal module failed, retrying..."
modprobe -r moal

# Create a lock file, as rebinding the mmc node will trigger the udev rules
# Do not remove the file at the end, it will be called by the script in the rebind call
touch $LOCKFILE

# Rebind and load the driver. Use a custom sleep to give enough time to the driver load.
rebind_mmc_node && load_and_check 2 && log "Wi-Fi activated" && exit 0

log "[ERROR] Cannot activate Wi-Fi"
exit 1
