# Copyright (C) 2023,2024, Digi International Inc.

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
	install -m 0644 murata/files/2DL/* ${D}${nonarch_base_libdir}/firmware/nxp
	# For the EU BT power file, set the baudrate to 3Mbps (as used by the btnxpuart driver)
	sed -i -e "s,00 C2 01 00 00 00 00 00 00 00 00 00,C0 C6 2D 00 00 00 00 00 00 00 00 00,g" \
		  ${D}${nonarch_base_libdir}/firmware/nxp/bt_power_config_EU.sh
	# For all the BT power scripts, replace the hcitool reset command to avoid misleading BT behaviour.
	sed -i -e "s,hcitool -i hci0 cmd 0x03 0x003,hciconfig hci0 reset,g" \
		  ${D}${nonarch_base_libdir}/firmware/nxp/bt_power_config*.sh
}

FILES:${PN} = "${nonarch_base_libdir}/firmware"

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "ccimx93"
