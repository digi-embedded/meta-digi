# Copyright (C) 2016 Freescale Semiconductor
# Copyright 2017-2018 NXP
# Copyright (C) 2018-2021 Digi International.

DESCRIPTION = "i.MX System Controller Firmware, customized for Digi platforms"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://COPYING;md5=e9e880185bda059c90c541d40ceca922"
SECTION = "BSP"

inherit pkgconfig deploy

SRC_URI = "${DIGI_PKG_SRC}/${BPN}-${PV}.tar.gz"

SRC_URI[md5sum] = "64419361dd592b9b9ebfc0d8bc1c6a75"
SRC_URI[sha256sum] = "38fc97904ad3da3caa62ae2ce1a8fcd036484473a3784117eec55b71201937f3"

S = "${WORKDIR}/${PN}-${PV}"

SC_FIRMWARE_NAME ?= "mx8x-${DIGI_FAMILY}-scfw-tcm.bin"
symlink_name = "scfw_tcm.bin"

SYSROOT_DIRS += "/boot"

do_install () {
    install -d ${D}/boot
    for ramc in ${RAM_CONFIGS}; do
        install -m 0644 ${S}/${SC_FIRMWARE_NAME}-${ramc} ${D}/boot/
    done
}

BOOT_TOOLS = "imx-boot-tools"

do_deploy () {
    install -d ${DEPLOYDIR}/${BOOT_TOOLS}
    for ramc in ${RAM_CONFIGS}; do
        install -m 0644 ${S}/${SC_FIRMWARE_NAME}-${ramc} ${DEPLOYDIR}/${BOOT_TOOLS}/
        cd ${DEPLOYDIR}/${BOOT_TOOLS}/
        rm -f ${symlink_name}-${ramc}
        ln -sf ${SC_FIRMWARE_NAME}-${ramc} ${symlink_name}-${ramc}
        cd -
    done
}

addtask deploy after do_install

INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
PACKAGE_ARCH = "${MACHINE_ARCH}"

FILES_${PN} = "/boot"

COMPATIBLE_MACHINE = "(ccimx8x)"
