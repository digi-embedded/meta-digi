# Copyright 2024 Digi International Inc.
FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI:append:ccimx93 = " file://ethosu_firmware"

do_install:ccimx93 () {
    install -d ${D}${nonarch_base_libdir}/firmware
    install -m 0644 ${WORKDIR}/ethosu_firmware ${D}${nonarch_base_libdir}/firmware/ethosu_firmware
}
