# Copyright (C) 2016-2021 Digi International.

SUMMARY = "Qualcomm firmware files for Digi's platforms."
SECTION = "base"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${DIGI_EULA_FILE};md5=8c0ad592dd48ace3d25eed5bbb26ba78"

# Selects whether to use proprietary or community driver
QUALCOMM_WIFI_DRIVER ?= "proprietary"

FW_QUALCOMM_BT = " \
    file://qca/nvm_tlv_3.0.bin \
    file://qca/nvm_tlv_3.2.bin \
    file://qca/rampatch_tlv_3.0.tlv \
    file://qca/rampatch_tlv_3.2.tlv \
"

# Firmware files for QCA6564 (Qualcomm proprietary driver)
FW_QCA6564_WIFI_PROPRIETARY = " \
    file://bdwlan30_US.bin \
    file://LICENCE.atheros_firmware \
    file://otp30.bin \
    file://qwlan30.bin \
    file://utf30.bin \
"

# Firmware files for QCA6574 (Qualcomm proprietary driver)
FW_QCA6574_WIFI_PROPRIETARY = " \
    file://LICENCE.atheros_firmware \
    file://qca6574_proprietary/qwlan30.bin \
    file://qca6574_proprietary/fakeboar.bin \
    file://qca6574_proprietary/otp.bin \
    file://qca6574_proprietary/utf.bin \
"

# Firmware files for QCA6574 (Qualcomm community driver)
# NOTE: the community file 'board.bin' must be substituted by proprietary
# 'fakeboar.bin'
FW_QCA6574_WIFI_COMMUNITY = " \
    file://qca6574_community/board-2.bin \
    file://qca6574_community/firmware-4.bin \
    file://qca6574_community/firmware-6.bin \
    file://qca6574_community/notice_ath10k_firmware-4.txt \
    file://qca6574_community/notice_ath10k_firmware-6.txt \
    file://qca6574_proprietary/fakeboar.bin \
"

FW_QUALCOMM_WIFI ?= "${FW_QCA6564_WIFI_PROPRIETARY}"
FW_QUALCOMM_WIFI_ccimx8x = "${@base_conditional('QUALCOMM_WIFI_DRIVER', 'community', '${FW_QCA6574_WIFI_COMMUNITY}', '${FW_QCA6574_WIFI_PROPRIETARY}', d)}"

SRC_URI = " \
    ${FW_QUALCOMM_BT} \
    ${FW_QUALCOMM_WIFI} \
"

S = "${WORKDIR}"

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
		# proprietary 'fakeboar.bin'
		ln -s fakeboar.bin ${D}${WIFI_FW_PATH}/board.bin
	else
		if [ "${FW_QUALCOMM_WIFI}" = "${FW_QCA6574_WIFI_PROPRIETARY}" ]; then
			ln -s qwlan30.bin ${D}${WIFI_FW_PATH}/athwlan.bin
			ln -s otp.bin ${D}${WIFI_FW_PATH}/athsetup.bin
		fi
	fi
}

QCA_MODEL ?= "qca6564"
QCA_MODEL_ccimx8x = "qca6574"

# Do not create empty debug and development packages (PN-dbg PN-dev PN-staticdev)
PACKAGES = "${PN}-${QCA_MODEL}-bt ${PN}-${QCA_MODEL}-wifi"

FILES_${PN}-${QCA_MODEL}-bt = "/lib/firmware/qca"
FILES_${PN}-${QCA_MODEL}-wifi = "/lib/firmware"

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccimx6$|ccimx6ul|ccimx8x)"
