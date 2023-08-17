# Copyright 2022 NXP
SUMMARY = "Freescale i.MX firmware for i.MX 9 family"
DESCRIPTION = "Freescale i.MX firmware for i.MX 9 family"

#
# Use meta-freescale's firmware-imx-8.18.inc and ammend license and
# SRC_URI checksums
#
# require recipes-bsp/firmware-imx/firmware-imx-${PV}.inc
require recipes-bsp/firmware-imx/firmware-imx-8.18.inc

LIC_FILES_CHKSUM = "file://COPYING;md5=63a38e9f392d8813d6f1f4d0d6fbe657"
SRC_URI[md5sum] = "25c50f3371450b2324401ee06ff1bf6a"
SRC_URI[sha256sum] = "f6dc6a5c8fd9b913a15360d3ccd53d188db05a08a8594c518e57622478c72383"

inherit deploy

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
