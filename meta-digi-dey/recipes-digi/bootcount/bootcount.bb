# Copyright (C) 2023 Digi International

SUMMARY = "Application to manage the bootcount value"
LICENSE = "MPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MPL-2.0;md5=815ca599c9df247a0c7f619bab123dad"

DEPENDS = "libubootenv"

PV = "1.0"

SRC_URI = " \
    file://bootcount-bin \
    file://bootcount-init/bootcount-init \
    file://bootcount-init/bootcount-init.service \
"

S = "${WORKDIR}/bootcount-bin"

inherit pkgconfig systemd update-rc.d

do_install() {
	oe_runmake DESTDIR=${D} install

	# INITSCRIPT
	install -d ${D}${sysconfdir}/init.d/
	install -m 0755 ${WORKDIR}/bootcount-init/bootcount-init ${D}${sysconfdir}/bootcount-init
	ln -sf /etc/bootcount-init ${D}${sysconfdir}/init.d/bootcount-init

	# SYSTEMD
	install -d ${D}${systemd_unitdir}/system/
	install -m 0644 ${WORKDIR}/bootcount-init/bootcount-init.service ${D}${systemd_unitdir}/system/
}

FILES:${PN} += " \
    ${sysconfdir}/bootcount-init \
    ${sysconfdir}/init.d/bootcount-init \
    ${systemd_unitdir}/system/bootcount-init.service \
"

INITSCRIPT_PACKAGES += "${PN}"
INITSCRIPT_NAME:${PN} = "bootcount-init"
INITSCRIPT_PARAMS:${PN} = "start 19 2 3 4 5 . stop 21 0 1 6 ."

SYSTEMD_SERVICE:${PN} = "bootcount-init.service"
