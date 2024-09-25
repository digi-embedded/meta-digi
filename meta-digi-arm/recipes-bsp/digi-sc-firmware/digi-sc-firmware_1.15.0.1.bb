# Copyright (C) 2016 Freescale Semiconductor
# Copyright 2017-2018 NXP
# Copyright (C) 2018-2024, Digi International Inc.

DESCRIPTION = "i.MX System Controller Firmware, customized for Digi platforms"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://COPYING;md5=ea25d099982d035af85d193c88a1b479"
SECTION = "BSP"

inherit pkgconfig deploy

SRC_URI = "${DIGI_PKG_SRC}/${BPN}-${PV}.tar.gz"

SRC_URI[md5sum] = "b7b9b5598b9ffc3a0f1663b41992aac0"
SRC_URI[sha256sum] = "59ebc2ebbf75c0b96f4fb70f8209f796543c5932e58f09031b9df99f110edcc9"

S = "${WORKDIR}/${PN}-${PV}"

SC_FIRMWARE_NAME ?= "mx8x-${DIGI_SOM}-scfw-tcm.bin"
symlink_name = "scfw_tcm.bin"

SYSROOT_DIRS += "/boot"

do_install () {
    install -d ${D}/boot
    install -m 0644 ${S}/${SC_FIRMWARE_NAME} ${D}/boot/

}

BOOT_TOOLS = "imx-boot-tools"

do_deploy () {
    install -d ${DEPLOYDIR}/${BOOT_TOOLS}
    install -m 0644 ${S}/${SC_FIRMWARE_NAME} ${DEPLOYDIR}/${BOOT_TOOLS}/
    cd ${DEPLOYDIR}/${BOOT_TOOLS}/
    rm -f ${symlink_name}
    ln -sf ${SC_FIRMWARE_NAME} ${symlink_name}
    cd -
}

addtask deploy after do_install

INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
PACKAGE_ARCH = "${MACHINE_ARCH}"

FILES:${PN} = "/boot"

COMPATIBLE_MACHINE = "(ccimx8x)"
