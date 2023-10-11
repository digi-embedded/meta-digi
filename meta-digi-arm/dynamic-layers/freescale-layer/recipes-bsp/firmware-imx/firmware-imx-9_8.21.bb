# Copyright 2022 NXP
SUMMARY = "Freescale i.MX firmware for i.MX 9 family"
DESCRIPTION = "Freescale i.MX firmware for i.MX 9 family"

#
# Use meta-freescale's firmware-imx-8.18.inc and ammend license and
# SRC_URI checksums
#
# require recipes-bsp/firmware-imx/firmware-imx-${PV}.inc
require recipes-bsp/firmware-imx/firmware-imx-8.18.inc

LIC_FILES_CHKSUM = "file://COPYING;md5=db4762b09b6bda63da103963e6e081de"
SRC_URI[md5sum] = "48b9e116280d752f1696dc36b08b07da"
SRC_URI[sha256sum] = "c3447f0f813415ccea9dc2ef12080cb3ac8bbc0c67392a74fc7d59205eb5a672"

inherit deploy nopackages

do_install[noexec] = "1"

do_deploy() {
    # Synopsys DDR
    for ddr_firmware in ${DDR_FIRMWARE_NAME}; do
        install -m 0644 ${S}/firmware/ddr/synopsys/${ddr_firmware} ${DEPLOYDIR}
    done
}
addtask deploy after do_install before do_build

PACKAGE_ARCH = "${MACHINE_SOCARCH}"

COMPATIBLE_MACHINE = "(mx9-nxp-bsp)"
