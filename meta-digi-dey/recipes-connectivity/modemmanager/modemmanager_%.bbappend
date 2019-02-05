# Copyright 2017, Digi International Inc.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI += " \
    file://78-mm-digi-xbee-cellular.rules \
    file://80-mm-net-device-blacklist.rules \
"

# 'polkit' depends on 'consolekit', and this requires 'x11' distro feature. So
# remove 'polkit' support to be able to build ModemManager on a framebuffer
# only image (without X11)
PACKAGECONFIG_remove = " polkit"

do_install_append() {
	# Install udev rules for XBee cellular
	install -d ${D}${nonarch_base_libdir}/udev/rules.d
	install -m 0644 ${WORKDIR}/78-mm-digi-xbee-cellular.rules ${D}${nonarch_base_libdir}/udev/rules.d/

	# Install udev rules for ModemManager blacklist devices
	install -m 0644 ${WORKDIR}/80-mm-net-device-blacklist.rules ${D}${nonarch_base_libdir}/udev/rules.d/
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
