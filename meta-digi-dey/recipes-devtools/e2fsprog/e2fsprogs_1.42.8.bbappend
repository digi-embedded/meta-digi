# Copyright (C) 2014 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}-${PV}:"

SRC_URI += "file://resize-ext4fs.sh"

inherit update-rc.d

do_install_append() {
	install -d ${D}${sysconfdir}/init.d
	install -m 0755 ${WORKDIR}/resize-ext4fs.sh ${D}${sysconfdir}/init.d/
}

INITSCRIPT_NAME = "resize-ext4fs.sh"
INITSCRIPT_PARAMS = "start 36 S ."
