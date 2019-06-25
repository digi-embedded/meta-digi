# Copyright (C) 2016 Freescale Semiconductor
# Copyright 2017-2018 NXP
# Copyright (C) 2018-2019 Digi International.

DESCRIPTION = "i.MX System Controller Firmware, customized for Digi platforms"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://COPYING;md5=fb0303e4ee8b0e71c094171e2272bd44"
SECTION = "BSP"

inherit pkgconfig deploy

SRC_URI = "${DIGI_PKG_SRC}/${PN}-${PV}.tar.gz"

SRC_URI[md5sum] = "18e4d7e6175792ff30d38aacbee3889e"
SRC_URI[sha256sum] = "121014d985a781be1feee1ecc65db0f8a373925e8fba606f1a286202a1e0c266"

S = "${WORKDIR}/${PN}-${PV}"

SC_FIRMWARE_NAME ?= "mx8qx-${DIGI_FAMILY}-scfw-tcm.bin"
symlink_name = "scfw_tcm.bin"

SYSROOT_DIRS += "/boot"

do_install () {
    install -d ${D}/boot
    for type in ${UBOOT_CONFIG}; do
        RAM_SIZE="$(echo ${type} | sed -e 's,.*\([0-9]\+GB\),\1,g')"
        install -m 0644 ${S}/${SC_FIRMWARE_NAME}-${RAM_SIZE} ${D}/boot/
    done
}

BOOT_TOOLS = "imx-boot-tools"

do_deploy () {
    install -d ${DEPLOYDIR}/${BOOT_TOOLS}
    for type in ${UBOOT_CONFIG}; do
        RAM_SIZE="$(echo ${type} | sed -e 's,.*\([0-9]\+GB\),\1,g')"
        install -m 0644 ${S}/${SC_FIRMWARE_NAME}-${RAM_SIZE} ${DEPLOYDIR}/${BOOT_TOOLS}/
        cd ${DEPLOYDIR}/${BOOT_TOOLS}/
        rm -f ${symlink_name}-${RAM_SIZE}
        ln -sf ${SC_FIRMWARE_NAME}-${RAM_SIZE} ${symlink_name}-${RAM_SIZE}
        cd -
    done
}

addtask deploy after do_install

INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
PACKAGE_ARCH = "${MACHINE_ARCH}"

FILES_${PN} = "/boot"

COMPATIBLE_MACHINE = "(ccimx8x)"
