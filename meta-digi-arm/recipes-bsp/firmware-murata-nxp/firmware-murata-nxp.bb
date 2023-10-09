# Copyright (C) 2023 Digi International.

SUMMARY = "Murata NXP firmware binaries"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://LICENSE;md5=ffa10f40b98be2c2bc9608f56827ed23"

SRCBRANCH = "imx-6-1-1"
SRCREV = "6103e224be638f5b421c323993f29bb6c0ada44a"
SRC_URI = "git://github.com/murata-wireless/nxp-linux-calibration;protocol=http;branch=${SRCBRANCH}"

S = "${WORKDIR}/git"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install () {
	install -d ${D}${nonarch_base_libdir}/firmware/nxp
	install -m 0644 murata/files/2DL/txpower_US.bin ${D}${nonarch_base_libdir}/firmware/nxp
	install -m 0644 murata/files/2DL/rutxpower_US.bin ${D}${nonarch_base_libdir}/firmware/nxp
	install -m 0755 murata/files/2DL/bt_power_config_US_CA_JP.sh ${D}${nonarch_base_libdir}/firmware/nxp
}

FILES:${PN} = "${nonarch_base_libdir}/firmware"

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "ccimx93"
