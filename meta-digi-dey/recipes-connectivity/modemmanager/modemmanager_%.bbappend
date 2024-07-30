# Copyright (C) 2017, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI += " \
    file://80-mm-net-device-blacklist.rules \
    file://late-modems-scan.service \
    file://late-modems-scan.timer \
"

# 'polkit' depends on 'consolekit', and this requires 'x11' distro feature. So
# remove 'polkit' support to be able to build ModemManager on a framebuffer
# only image (without X11)
PACKAGECONFIG:remove:dey = " polkit"

do_install:append() {
	# Install udev rules for ModemManager blacklist devices
	install -d ${D}${nonarch_base_libdir}/udev/rules.d
	install -m 0644 ${WORKDIR}/80-mm-net-device-blacklist.rules ${D}${nonarch_base_libdir}/udev/rules.d/

	# Install systemd service for scanning late modems
	install -d ${D}${systemd_unitdir}/system/
	install -m 0644 ${WORKDIR}/late-modems-scan.service ${D}${systemd_unitdir}/system/
	install -m 0644 ${WORKDIR}/late-modems-scan.timer ${D}${systemd_unitdir}/system/
}

SYSTEMD_SERVICE:${PN}:append = " late-modems-scan.timer"
FILES:${PN}:append = " late-modems-scan.timer"

PACKAGE_ARCH = "${MACHINE_ARCH}"
