# Copyright (C) 2023,2024, Digi International Inc.

LIC_FILES_CHKSUM = "file://LICENSE.txt;md5=ca53281cc0caa7e320d4945a896fb837"

SRCBRANCH = "lf-6.6.36_2.1.0"
SRCREV = "1b26d19284d202b1531837ce37a05afc49ad1d98"

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
