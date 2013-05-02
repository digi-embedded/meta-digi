# Copyright (C) 2012 Digi International

DESCRIPTION = "bootloader for Digi platforms"

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"

# Uncomment to build the from sources (internal use only)
# UBOOT_BUILD_SRC ?= "1"

SRCREV_mxs = "af1375fd2b7273ab6e7c8a2506a58605264b6585"
SRCREV_mx5 = "af1375fd2b7273ab6e7c8a2506a58605264b6585"
SRCREV_SHORT = "${@'${SRCREV}'[:7]}"

# Checksums for 'u-boot-denx-${SRCREV_SHORT}.tar.gz' tarballs
TARBALL_MD5    = "820bafe66d61b3b511c34683e9be7442"
TARBALL_SHA256 = "4dcbe44e5b664f8742d546bb107dc96708bb43c5f058cf8d5198bbd100f206bb"

SRC_URI_git = "${DIGI_LOG_GIT}u-boot-denx.git;protocol=git;branch=refs/heads/master"
SRC_URI_tarball = " \
        ${DIGI_MIRROR}/u-boot-denx-${SRCREV_SHORT}.tar.gz;md5sum=${TARBALL_MD5};sha256sum=${TARBALL_SHA256} \
        "

SRC_URI  = "${@base_conditional('UBOOT_BUILD_SRC', '1' , '${SRC_URI_git}', '${SRC_URI_tarball}', d)}"

S  = "${@base_conditional('UBOOT_BUILD_SRC', '1' , '${WORKDIR}/git', '${WORKDIR}/u-boot-denx-${SRCREV_SHORT}', d)}"

DEPENDS_mxs += "elftosb-native imx-bootlets-del"

BOOTLETSDIR_mxs = "BOOTLETS_DIR=${STAGING_DIR_TARGET}/boot/"
EXTRA_OEMAKE += '${BOOTLETSDIR}'

# The meta-fsl-arm recipe overrides this so it needs to be done here again.
UBOOT_MAKE_TARGET_ccardimx28js = "u-boot-ivt.sb"
UBOOT_MAKE_TARGET_cpx2 = "u-boot-ivt.sb"

COMPATIBLE_MACHINE = "(ccardimx28js|ccimx51js|ccimx53js|cpx2)"
