# Copyright (C) 2012 Digi International

SUMMARY = "IMX bootlets for Digi platforms"
SECTION = "base"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

PR = "${DISTRO}.r0"

SRCREV_external = ""
SRCREV_internal = "831e2d7f6db875be9c65f45a7a594aff8b560029"
SRCREV = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRCREV_internal}', '${SRCREV_external}', d)}"

SRC_URI_external = "${DIGI_GITHUB_GIT}/imx-bootlets.git;protocol=git"
SRC_URI_internal = "${DIGI_MTK_GIT}linux/imx-bootlets.git;protocol=ssh"
SRC_URI = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRC_URI_internal}', '${SRC_URI_external}', d)}"

S = "${WORKDIR}/git"

# Disable parallel building or it may fail to build.
PARALLEL_MAKE = ""

EXTRA_OEMAKE = "CROSS_COMPILE=${TARGET_PREFIX} BOARD=${IMXBOOTLETS_MACHINE}"

# Ensure machine defines the IMXBOOTLETS_MACHINE
python () {
    if not d.getVar("IMXBOOTLETS_MACHINE", True):
        PN = d.getVar("PN", True)
        FILE = os.path.basename(d.getVar("FILE", True))
        bb.debug(1, "To build %s, see %s for instructions on \
                     setting up your machine config" % (PN, FILE))
        raise bb.parse.SkipPackage("because IMXBOOTLETS_MACHINE is not set")
}

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
