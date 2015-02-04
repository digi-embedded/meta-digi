# Copyright (C) 2012 Digi International

DESCRIPTION = "Bootloader for Digi platforms"
require recipes-bsp/u-boot/u-boot.inc
include u-boot-dey-rev_${PV}.inc

PROVIDES += "u-boot"

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://COPYING;md5=1707d6db1d42237583f50183a5651ecb"

SRC_URI += "file://boot-sd.txt"

S = "${WORKDIR}/git"

do_compile_prepend() {
	${S}/tools/setlocalversion --save-scmversion ${S}
}

do_deploy_append() {
	sed -i -e 's,##CPU_FAMILY##,${CPU_FAMILY},g' ${WORKDIR}/boot-sd.txt
	mkimage -T script -n bootscript -C none -d ${WORKDIR}/boot-sd.txt ${DEPLOYDIR}/boot-sd.scr
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccimx6)"
