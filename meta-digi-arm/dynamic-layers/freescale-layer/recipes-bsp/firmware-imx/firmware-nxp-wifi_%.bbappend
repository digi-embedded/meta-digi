# Copyright (C) 2023 Digi International.

LIC_FILES_CHKSUM:ccimx93 = "file://LICENSE.txt;md5=db4762b09b6bda63da103963e6e081de"

SRCBRANCH:ccimx93 = "lf-6.1.36_2.1.0"
SRCREV:ccimx93 = "1fb80d0266e8044fb7eea695c7678cddcbbc77c5"

do_install:append:ccimx93() {
	# Install NXP Connectivity IW612 firmware
	install -m 0644 nxp/FwImage_IW612_SD/sd_w61x_v1.bin.se  ${D}${nonarch_base_libdir}/firmware/nxp
	install -m 0644 nxp/FwImage_IW612_SD/uartspi_n61x_v1.bin.se ${D}${nonarch_base_libdir}/firmware/nxp
}

PACKAGES:prepend:ccimx93 = "${PN}-nxpiw612 "

FILES:${PN}-nxpiw612 = " \
    ${nonarch_base_libdir}/firmware/nxp/sd_w61x_v1.bin.se \
    ${nonarch_base_libdir}/firmware/nxp/uartspi_n61x_v1.bin.se \
"
