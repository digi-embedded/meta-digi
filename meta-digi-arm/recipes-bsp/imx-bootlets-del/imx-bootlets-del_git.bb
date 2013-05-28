# Copyright (C) 2012 Digi International

SUMMARY = "IMX bootlets for Digi platforms"
SECTION = "base"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

PR = "${DISTRO}.r0"

# Uncomment to build the from sources (internal use only)
# IMX_BOOTLETS_BUILD_SRC ?= "1"

# Checksums for 'imx-bootlets-${SRCREV_SHORT}.tar.gz' tarballs
TARBALL_MD5    = "457c5925dbc5d3ee0839f1dfdb0b7d37"
TARBALL_SHA256 = "eae8c6d7b872a7bb1c689010a11890918f4187a87e1c521f3b2bd80f63a8dd77"

SRCREV = "cc3b1eb94dda62aa737f2289b7a2d3936492a53b"
SRCREV_SHORT = "${@'${SRCREV}'[:7]}"
SRC_URI_git = "${DIGI_MTK_GIT}del/imx-bootlets.git;branch=refs/heads/master;protocol=git"
SRC_URI_tarball = " \
        ${DIGI_MIRROR}/imx-bootlets-${SRCREV_SHORT}.tar.gz;md5sum=${TARBALL_MD5};sha256sum=${TARBALL_SHA256} \
        "

SRC_URI  = "${@base_conditional('IMX_BOOTLETS_BUILD_SRC', '1' , '${SRC_URI_git}', '${SRC_URI_tarball}', d)}"
S  = "${@base_conditional('IMX_BOOTLETS_BUILD_SRC', '1' , '${WORKDIR}/git', '${WORKDIR}/imx-bootlets-${SRCREV_SHORT}', d)}"

# Disable parallel building or it may fail to build.
PARALLEL_MAKE = ""

EXTRA_OEMAKE = "CROSS_COMPILE=${TARGET_PREFIX}"
EXTRA_OEMAKE_append_ccardimx28js = " BOARD=CCARDIMX28JS"
EXTRA_OEMAKE_append_cpx2 = " BOARD=CPX2"
EXTRA_OEMAKE_append_wr21 = " BOARD=WR21"

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
COMPATIBLE_MACHINE = "(ccardimx28js|cpx2|wr21)"
