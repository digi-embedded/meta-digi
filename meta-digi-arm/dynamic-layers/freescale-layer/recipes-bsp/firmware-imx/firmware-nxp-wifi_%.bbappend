# Copyright (C) 2023,2024, Digi International Inc.

LIC_FILES_CHKSUM:ccimx9 = "file://LICENSE.txt;md5=2827219e81f28aba7c6a569f7c437fa7"

SRCBRANCH:ccimx9 = "lf-6.1.55_2.2.0"
SRCREV:ccimx9 = "7be5a936ce8677962dd7b41c6c9f41dd14350bec"

do_install:append:ccimx9() {
	# Install NXP Connectivity IW612 firmware
	install -m 0644 nxp/FwImage_IW612_SD/sd_w61x_v1.bin.se  ${D}${nonarch_base_libdir}/firmware/nxp
	install -m 0644 nxp/FwImage_IW612_SD/uartspi_n61x_v1.bin.se ${D}${nonarch_base_libdir}/firmware/nxp
}

PACKAGES:prepend:ccimx9 = "${PN}-nxpiw612 "

FILES:${PN}-nxpiw612 = " \
    ${nonarch_base_libdir}/firmware/nxp/sd_w61x_v1.bin.se \
    ${nonarch_base_libdir}/firmware/nxp/uartspi_n61x_v1.bin.se \
"
