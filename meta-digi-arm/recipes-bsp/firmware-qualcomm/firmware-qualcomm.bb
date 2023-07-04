# Copyright (C) 2016-2023 Digi International.

SUMMARY = "Qualcomm firmware files for Digi's platforms."
SECTION = "base"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${DIGI_EULA_FILE};md5=8c0ad592dd48ace3d25eed5bbb26ba78"

# Selects whether to use proprietary or community driver
QUALCOMM_WIFI_DRIVER ?= "proprietary"

# Selects whether to apply the "Deep Sleep + Internal Clock" workaround
BT_INTCLOCK_WORKAROUND ?= "0"
BT_INTCLOCK_WORKAROUND:ccimx6ul = "1"
BT_INTCLOCK_WORKAROUND:ccimx6 = "1"

# Bluetooth 5.0 firmware files
FW_QUALCOMM_BT_5 = " \
    file://qca65X4_bt/nvm_tlv_3.2.bin \
    file://qca65X4_bt/rampatch_tlv_3.2.tlv \
"

# Bluetooth 4.1 firmware files
FW_QUALCOMM_BT_4 = " \
    file://qca65X4_bt_4/nvm_tlv_3.2.bin \
    file://qca65X4_bt_4/rampatch_tlv_3.2.tlv \
"

FW_QUALCOMM_BT = "${@oe.utils.conditional('BT_FW_VERSION', '4', '${FW_QUALCOMM_BT_4}', '${FW_QUALCOMM_BT_5}', d)}"

# Firmware files for QCA6564 (Qualcomm proprietary driver)
FW_QCA65X4_SDIO_PROPRIETARY = " \
    file://bdwlan30_US.bin \
    file://LICENCE.atheros_firmware \
    file://qca65X4_sdio_proprietary/otp30.bin \
    file://qca65X4_sdio_proprietary/qwlan30.bin \
    file://qca65X4_sdio_proprietary/utf30.bin \
"
FW_QCA65X4_SDIO_CCX = "file://qca65X4_sdio_proprietary/qwlan30-ccx.bin"

# Firmware files for QCA6574 (Qualcomm proprietary driver)
FW_QCA65X4_PCIE_PROPRIETARY = " \
    file://bdwlan30_US.bin \
    file://LICENCE.atheros_firmware \
    file://qca65X4_pcie_proprietary/otp30.bin \
    file://qca65X4_pcie_proprietary/qwlan30.bin \
    file://qca65X4_pcie_proprietary/utf.bin \
"
FW_QCA65X4_PCIE_CCX = ""

# Firmware files for QCA6574 (Qualcomm community driver)
# NOTE: the community file 'board.bin' must be substituted by proprietary
# 'bdwlan30_US.bin'
FW_QCA6574_WIFI_COMMUNITY = " \
    file://bdwlan30_US.bin \
    file://qca6574_community/board-2.bin \
    file://qca6574_community/firmware-4.bin \
    file://qca6574_community/firmware-6.bin \
    file://qca6574_community/notice_ath10k_firmware-4.txt \
    file://qca6574_community/notice_ath10k_firmware-6.txt \
"

FW_QUALCOMM_WIFI ?= "${FW_QCA65X4_SDIO_PROPRIETARY}"
FW_QUALCOMM_WIFI:ccimx8x = "${@oe.utils.conditional('QUALCOMM_WIFI_DRIVER', 'community', '${FW_QCA6574_WIFI_COMMUNITY}', '${FW_QCA65X4_PCIE_PROPRIETARY}', d)}"

FW_QUALCOMM_CCX ?= "${FW_QCA65X4_SDIO_CCX}"
FW_QUALCOMM_CCX:ccimx8x = "${@oe.utils.conditional('QUALCOMM_WIFI_DRIVER', 'community', '', '${FW_QCA65X4_PCIE_CCX}', d)}"

SRC_URI = " \
    ${FW_QUALCOMM_BT} \
    ${FW_QUALCOMM_WIFI} \
    ${@oe.utils.vartrue('QUALCOMM_FW_CCX_TAGS', '${FW_QUALCOMM_CCX}', '', d)} \
    file://nvm-tag33.bin \
"

S = "${WORKDIR}"

do_unpack[postfuncs] += "${@oe.utils.vartrue('QUALCOMM_FW_CCX_TAGS', 'copy_ccx_fw', '', d)}"
copy_ccx_fw() {
	FW_CCX_FILE="$(echo ${FW_QUALCOMM_CCX} | sed -e 's,file\:\/\/,,g')"
	FW_NOCCX_FILE="$(echo ${FW_CCX_FILE} | sed -e 's,-ccx,,g')"
	cp --remove-destination ${WORKDIR}/${FW_CCX_FILE} ${WORKDIR}/${FW_NOCCX_FILE}
}

do_install() {
	# BT firmware (remove 'file://' from variable with files list)
	BT_FW_FILES=$(echo ${FW_QUALCOMM_BT} | sed -e 's,file\:\/\/,,g')
	install -d ${D}${base_libdir}/firmware/qca
	install -m 0644 ${BT_FW_FILES} ${D}${base_libdir}/firmware/qca

	# Wifi firmware
	if [ "${QUALCOMM_WIFI_DRIVER}" = "community" ]; then
		WIFI_FW_PATH="${base_libdir}/firmware/ath10k/QCA6174/hw3.0"
	else
		WIFI_FW_PATH="${base_libdir}/firmware"
	fi
	install -d ${D}${WIFI_FW_PATH}
	# Remove preceeding 'file://' from variable with files list
	FW_WIFI_FILES="$(echo ${FW_QUALCOMM_WIFI} | sed -e 's,file\:\/\/,,g')"
	install -m 0644 ${FW_WIFI_FILES} ${D}${WIFI_FW_PATH}
	if [ "${QUALCOMM_WIFI_DRIVER}" = "community" ]; then
		# If using community driver, create symlink 'board.bin' to
		# proprietary 'bdwlan30_US.bin'
		ln -s bdwlan30_US.bin ${D}${WIFI_FW_PATH}/board.bin
	else
		# Create symbolic links between the mac addresses in the device tree nodes
		# and the paths where the driver expect the MAC addresses
		install -d ${D}${WIFI_FW_PATH}/wlan/
		ln -s /proc/device-tree/wireless/mac-address ${D}${WIFI_FW_PATH}/wlan/wlan_mac0
		ln -s /proc/device-tree/wireless/mac-address1 ${D}${WIFI_FW_PATH}/wlan/wlan_mac1
		ln -s /proc/device-tree/wireless/mac-address2 ${D}${WIFI_FW_PATH}/wlan/wlan_mac2
		ln -s /proc/device-tree/wireless/mac-address3 ${D}${WIFI_FW_PATH}/wlan/wlan_mac3

		if [ "${FW_QUALCOMM_WIFI}" = "${FW_QCA65X4_PCIE_PROPRIETARY}" ]; then
			ln -s qwlan30.bin ${D}${WIFI_FW_PATH}/athwlan.bin
			ln -s otp30.bin ${D}${WIFI_FW_PATH}/athsetup.bin
		fi
	fi

	# Disable IBS over H4 for all the platforms in the bluetooth firmware
	awk 'BEGIN{printf "%c", 0x02}' | dd of="${D}${base_libdir}/firmware/qca/nvm_tlv_3.2.bin" bs=1 seek=54 count=1 conv=notrunc,fsync

	if "${@oe.utils.conditional('BT_INTCLOCK_WORKAROUND', '1', 'true', 'false', d)}"; then
		# Disable DEEP SLEEP in the bluetooth firmware
		awk 'BEGIN{printf "%c", 0x00}' | dd of="${D}${base_libdir}/firmware/qca/nvm_tlv_3.2.bin" bs=1 seek=74 count=1 conv=notrunc,fsync
		# Enable Internal Clock in the bluetooth firmware
		awk 'BEGIN{printf "%c%c", 0x01, 0x00}' | dd of="${D}${base_libdir}/firmware/qca/nvm_tlv_3.2.bin" bs=1 seek=93 count=2 conv=notrunc,fsync
	fi

	# Insert TAG33 for CVE-2019-9506 and update file length
	cat nvm-tag33.bin >> "${D}${base_libdir}/firmware/qca/nvm_tlv_3.2.bin"
	# Calculate the new firmware file size
	length="$(expr $(stat -L -c %s ${D}${base_libdir}/firmware/qca/nvm_tlv_3.2.bin) - 4)"
	/bin/echo -ne "\x$(printf '%02x' $(expr $length % 256))" | dd of=${D}${base_libdir}/firmware/qca/nvm_tlv_3.2.bin bs=1 seek=1 count=1 conv=notrunc,fsync
	/bin/echo -ne "\x$(printf '%02x' $(expr $length / 256))" | dd of=${D}${base_libdir}/firmware/qca/nvm_tlv_3.2.bin bs=1 seek=2 count=1 conv=notrunc,fsync
}

QCA_MODEL ?= "qca6564"
QCA_MODEL:ccimx8x = "qca6574"

# Do not create empty debug and development packages (PN-dbg PN-dev PN-staticdev)
PACKAGES = "${PN}-${QCA_MODEL}-bt ${PN}-${QCA_MODEL}-wifi"

FILES:${PN}-${QCA_MODEL}-bt = "/lib/firmware/qca"
FILES:${PN}-${QCA_MODEL}-wifi = "/lib/firmware"

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccimx6$|ccimx6ul|ccimx8x|ccimx8m)"
