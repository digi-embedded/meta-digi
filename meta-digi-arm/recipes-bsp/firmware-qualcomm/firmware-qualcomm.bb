# Copyright (C) 2016-2021 Digi International.

SUMMARY = "Qualcomm firmware files for Digi's platforms."
SECTION = "base"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${DIGI_EULA_FILE};md5=4c0991cfde5c8a92338cbfe0f4f9a5c6"

FW_QCA6564-BT = " \
    file://qca/nvm_tlv_3.0.bin \
    file://qca/nvm_tlv_3.2.bin \
    file://qca/rampatch_tlv_3.0.tlv \
    file://qca/rampatch_tlv_3.2.tlv \
"

FW_QCA6564-WIFI = " \
    file://bdwlan30_US.bin \
    file://bdwlan30_World.bin \
    file://LICENCE.atheros_firmware \
    file://otp30.bin \
    file://qwlan30.bin \
    file://utf30.bin \
    file://wlan/cfg.dat \
    file://wlan/qcom_cfg.ini \
"

SRC_URI = " \
    ${FW_QCA6564-BT} \
    ${FW_QCA6564-WIFI} \
"

S = "${WORKDIR}"

do_install() {
	# BT firmware
	install -d ${D}${base_libdir}/firmware/qca
	install -m 0644 \
		qca/nvm_tlv_3.0.bin \
		qca/nvm_tlv_3.2.bin \
		qca/rampatch_tlv_3.0.tlv \
		qca/rampatch_tlv_3.2.tlv \
		${D}${base_libdir}/firmware/qca

	# Wifi firmware
	install -d ${D}${base_libdir}/firmware/wlan
	install -m 0644 \
		bdwlan30_US.bin \
		bdwlan30_World.bin \
		LICENCE.atheros_firmware \
		otp30.bin \
		qwlan30.bin \
		utf30.bin \
		${D}${base_libdir}/firmware
	install -m 0644 \
		wlan/cfg.dat \
		wlan/qcom_cfg.ini \
		${D}${base_libdir}/firmware/wlan
}

# Do not create empty debug and development packages (PN-dbg PN-dev PN-staticdev)
PACKAGES = "${PN}-qca6564-bt ${PN}-qca6564-wifi"

FILES_${PN}-qca6564-bt = "/lib/firmware/qca"
FILES_${PN}-qca6564-wifi = "/lib/firmware"

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccimx6$|ccimx6ul)"
