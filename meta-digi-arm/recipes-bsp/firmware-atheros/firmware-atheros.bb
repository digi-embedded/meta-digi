# Copyright (C) 2013 Digi International.

SUMMARY = "Firmware files for Digi's platforms, such as Atheros bluetooth."
SECTION = "base"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${DIGI_EULA_FILE};md5=4c0991cfde5c8a92338cbfe0f4f9a5c6"

FW_ATH6KL = " \
    file://athtcmd_ram.bin \
    file://athwlan.bin \
    file://Digi_6203_2_ANT-US.bin \
    file://Digi_6203_2_ANT-World.bin \
    file://Digi_6203-6233-US.bin \
    file://Digi_6203-6233-World.bin \
    file://fw-4.bin \
    file://nullTestFlow.bin \
    file://utf.bin \
"

FW_AR3K = " \
    file://PS_ASIC_class_1.pst \
    file://PS_ASIC_class_2.pst \
    file://RamPatch.txt \
    file://readme.txt \
"

SRC_URI = " \
    ${FW_AR3K} \
    ${FW_ATH6KL} \
"

S = "${WORKDIR}"

do_install() {
	# AR3K bluetooth firmware
	install -d ${D}${base_libdir}/firmware/ar3k/1020200
	install -m 0644 \
		PS_ASIC_class_1.pst \
		PS_ASIC_class_2.pst \
		RamPatch.txt \
		readme.txt \
		${D}${base_libdir}/firmware/ar3k/1020200/

	# ATH6KL wireless firmware
	install -d ${D}${base_libdir}/firmware/ath6k/AR6003/hw2.1.1
	install -m 0644 \
		athtcmd_ram.bin \
		athwlan.bin \
		Digi_6203_2_ANT-US.bin \
		Digi_6203_2_ANT-World.bin \
		Digi_6203-6233-US.bin \
		Digi_6203-6233-World.bin \
		fw-4.bin \
		nullTestFlow.bin \
		utf.bin \
		${D}${base_libdir}/firmware/ath6k/AR6003/hw2.1.1/
	# Wireless certification
	#     0x0 = US
	#     0x1 = International
	#     0x2 = Japan
	ln -sf /proc/device-tree/wireless/mac-address ${D}${base_libdir}/firmware/ath6k/AR6003/hw2.1.1/softmac
	ln -sf Digi_6203-6233-US.bin ${D}${base_libdir}/firmware/ath6k/AR6003/hw2.1.1/bdata.0x0.bin
	ln -sf Digi_6203-6233-World.bin ${D}${base_libdir}/firmware/ath6k/AR6003/hw2.1.1/bdata.0x1.bin
	ln -sf Digi_6203-6233-World.bin ${D}${base_libdir}/firmware/ath6k/AR6003/hw2.1.1/bdata.0x2.bin
	ln -sf Digi_6203_2_ANT-US.bin ${D}${base_libdir}/firmware/ath6k/AR6003/hw2.1.1/bdata.ANT-0x0.bin
	ln -sf Digi_6203_2_ANT-World.bin ${D}${base_libdir}/firmware/ath6k/AR6003/hw2.1.1/bdata.ANT-0x1.bin
	ln -sf Digi_6203_2_ANT-World.bin ${D}${base_libdir}/firmware/ath6k/AR6003/hw2.1.1/bdata.ANT-0x2.bin
}

# Do not create empty debug and development packages (PN-dbg PN-dev PN-staticdev)
PACKAGES = "${PN}-ar3k ${PN}-ath6kl"

FILES_${PN}-ar3k = "/lib/firmware/ar3k"
FILES_${PN}-ath6kl = "/lib/firmware/ath6k"

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccimx6$)"
