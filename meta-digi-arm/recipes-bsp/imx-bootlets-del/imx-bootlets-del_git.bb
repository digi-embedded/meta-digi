# Copyright (C) 2012 Digi International

DESCRIPTION = "IMX bootlets for Digi platforms"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

PR = "r0"


SRCREV="master"
SRC_URI = "${DIGI_MTK_GIT}del/imx-bootlets.git;"
S = "${WORKDIR}/git"

# Disable parallel building or it may fail to build.
PARALLEL_MAKE = ""

EXTRA_OEMAKE = "BOARD=CCARDIMX28JS CROSS_COMPILE=${TARGET_PREFIX}"

do_install () {
    install -d ${STAGING_DIR_TARGET}/boot/
    install -m 644 boot_prep/boot_prep power_prep/power_prep \
                   linux_prep/output-target/linux_prep \
		   uboot.bd uboot_ivt.bd linux.bd linux_ivt.bd \
                   ${STAGING_DIR_TARGET}/boot
    install -d ${D}/boot/
    install -m 644 boot_prep/boot_prep power_prep/power_prep \
                   linux_prep/output-target/linux_prep \
		   uboot.bd uboot_ivt.bd linux.bd linux_ivt.bd \
                   ${D}/boot
}

FILES_${PN} = "/boot"

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccardimx28js)"
