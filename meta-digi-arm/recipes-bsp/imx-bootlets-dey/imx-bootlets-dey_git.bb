# Copyright (C) 2012 Digi International

SUMMARY = "IMX bootlets for Digi platforms"
SECTION = "base"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

PR = "${DISTRO}.r0"

SRCREV_external = ""
SRCREV_internal = "d7573ef42237c078c7e7d646fced6243b53aee77"
SRCREV = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRCREV_internal}', '${SRCREV_external}', d)}"

SRC_URI_external = "${DIGI_GITHUB_GIT}/imx-bootlets.git;protocol=git"
SRC_URI_internal = "${DIGI_MTK_GIT}del/imx-bootlets.git;protocol=git"
SRC_URI = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRC_URI_internal}', '${SRC_URI_external}', d)}"

S = "${WORKDIR}/git"

# Disable parallel building or it may fail to build.
PARALLEL_MAKE = ""

EXTRA_OEMAKE = "CROSS_COMPILE=${TARGET_PREFIX}"
EXTRA_OEMAKE_append_ccardimx28js = " BOARD=CCARDIMX28JS"
EXTRA_OEMAKE_append_cpx2 = " BOARD=CPX2"
EXTRA_OEMAKE_append_wr21 = " BOARD=WR21"

do_install () {
	install -d ${STAGING_DIR_TARGET}/boot
	install -m 644 boot_prep/boot_prep power_prep/power_prep \
		linux_prep/output-target/linux_prep \
		uboot.bd uboot_ivt.bd linux.bd linux_ivt.bd \
		${STAGING_DIR_TARGET}/boot
	install -d ${D}/boot
	install -m 644 boot_prep/boot_prep power_prep/power_prep \
		linux_prep/output-target/linux_prep \
		uboot.bd uboot_ivt.bd linux.bd linux_ivt.bd \
		${D}/boot
}

FILES_${PN} = "/boot"

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(mxs)"
