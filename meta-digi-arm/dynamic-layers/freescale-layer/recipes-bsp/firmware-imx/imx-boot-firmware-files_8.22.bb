# Copyright 2022 NXP
SUMMARY = "Freescale i.MX firmware for i.MX 9 family"
DESCRIPTION = "Freescale i.MX firmware for i.MX 9 family"

#
# Use meta-freescale's firmware-imx-8.18.inc and ammend license and
# SRC_URI checksums
#
# require recipes-bsp/firmware-imx/firmware-imx-${PV}.inc
require recipes-bsp/firmware-imx/firmware-imx-8.18.inc

LIC_FILES_CHKSUM = "file://COPYING;md5=2827219e81f28aba7c6a569f7c437fa7"
SRC_URI[md5sum] = "c5cf3842569f0a7fd990fbc64979e84f"
SRC_URI[sha256sum] = "94c8bceac56ec503c232e614f77d6bbd8e17c7daa71d4e651ea8fd5034c30350"

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
