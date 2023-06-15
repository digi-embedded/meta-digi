# Copyright 2022 NXP
SUMMARY = "Freescale i.MX firmware for i.MX 9 family"
DESCRIPTION = "Freescale i.MX firmware for i.MX 9 family"

#
# Use meta-freescale's firmware-imx-8.18.inc and ammend license and
# SRC_URI checksums
#
# require recipes-bsp/firmware-imx/firmware-imx-${PV}.inc
require recipes-bsp/firmware-imx/firmware-imx-8.18.inc

LIC_FILES_CHKSUM = "file://COPYING;md5=ea25d099982d035af85d193c88a1b479"
SRC_URI[md5sum] = "5228cca9bac48a5fe733b886884cf2ab"
SRC_URI[sha256sum] = "a4102a48e8b9031a06036bdffd0a99e26216aad80f40e6cd4a3a5409be278bb5"

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
