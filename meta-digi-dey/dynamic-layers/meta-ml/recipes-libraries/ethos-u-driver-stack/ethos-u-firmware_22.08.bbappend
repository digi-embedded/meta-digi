# Copyright 2023 Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

# Use our custom ethosu_firmware without debug port; keep the commented info for traceability
#SRCBRANCH = "lf-6.1.1_1.0.0"
#SRCREV = "c80a413664f650c366fc4721474a3fe1d1503eb5"
SRC_URI:append:ccimx93 = " file://ethosu_firmware"

do_install:append:ccimx93 () {
    install -m 0644 ${WORKDIR}/ethosu_firmware ${D}${nonarch_base_libdir}/firmware
}
