# Copyright (C) 2016 Digi International.

SUMMARY = "Recovery initramfs files"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI = " \
    file://recovery-initramfs-init \
    file://swupdate.cfg \
"

S = "${WORKDIR}"

do_install() {
	install -d ${D}${sysconfdir}
	install -m 0755 ${WORKDIR}/recovery-initramfs-init ${D}/init
	install -m 0644 ${WORKDIR}/swupdate.cfg ${D}${sysconfdir}
}

# Do not create debug/devel packages
PACKAGES = "${PN}"

FILES_${PN} = "/"
