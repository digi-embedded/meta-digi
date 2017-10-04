# Copyright (C) 2016,2017 Digi International.

SUMMARY = "Qualcomm firmware files for Digi's platforms."
SECTION = "base"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${DIGI_EULA_FILE};md5=8c0ad592dd48ace3d25eed5bbb26ba78"

FW_QCA6564-BT = " \
    file://qca/nvm_tlv_3.0.bin \
    file://qca/nvm_tlv_3.2.bin \
    file://qca/rampatch_tlv_3.0.tlv \
    file://qca/rampatch_tlv_3.2.tlv \
"

FW_QCA6564-WIFI = " \
    file://bdwlan30_US.bin \
    file://LICENCE.atheros_firmware \
    file://otp30.bin \
    file://qwlan30.bin \
    file://utf30.bin \
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
	install -d ${D}${base_libdir}/firmware
	install -m 0644 \
		bdwlan30_US.bin \
		LICENCE.atheros_firmware \
		otp30.bin \
		qwlan30.bin \
		utf30.bin \
		${D}${base_libdir}/firmware
}

# Do not create empty debug and development packages (PN-dbg PN-dev PN-staticdev)
PACKAGES = "${PN}-qca6564-bt ${PN}-qca6564-wifi"

FILES_${PN}-qca6564-bt = "/lib/firmware/qca"
FILES_${PN}-qca6564-wifi = "/lib/firmware"

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccimx6qpsbc|ccimx6ul)"
