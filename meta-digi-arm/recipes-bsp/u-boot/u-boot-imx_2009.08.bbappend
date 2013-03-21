# Copyright (C) 2012 Digi International

DESCRIPTION = "bootloader for Digi platforms"

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"

# Uncomment to build the from sources (internal use only)
# UBOOT_BUILD_SRC ?= "1"

SRCREV_mxs = "57dc7fb843484ff0099f6e5f5a2528a7a2dd1766"
SRCREV_mx5 = "57dc7fb843484ff0099f6e5f5a2528a7a2dd1766"
SRCREV_SHORT = "${@'${SRCREV}'[:7]}"

# Checksums for 'u-boot-denx-${SRCREV_SHORT}.tar.gz' tarballs
TARBALL_MD5    = "a6d29ccd750719b82c366b5cbc890212"
TARBALL_SHA256 = "7b387f1e246e8c44cd561b68ba9c7182915e13b460b6f16b6b2013a6d6aaa302"

SRC_URI_git = "${DIGI_LOG_GIT}u-boot-denx.git;protocol=git;branch=refs/heads/master"
SRC_URI_tarball = " \
        ${DIGI_MIRROR}/u-boot-denx-${SRCREV_SHORT}.tar.gz;md5sum=${TARBALL_MD5};sha256sum=${TARBALL_SHA256} \
        "

SRC_URI  = "${@base_conditional('UBOOT_BUILD_SRC', '1' , '${SRC_URI_git}', '${SRC_URI_tarball}', d)}"

S  = "${@base_conditional('UBOOT_BUILD_SRC', '1' , '${WORKDIR}/git', '${WORKDIR}/u-boot-denx-${SRCREV_SHORT}', d)}"

DEPENDS_mxs_ccardimx28js += "elftosb-native imx-bootlets-del"
DEPENDS_mxs_cpx2 += "elftosb-native imx-bootlets-del"

UBOOT_MAKE_TARGET_ccardimx28js = "u-boot-ivt.sb"
UBOOT_MAKE_TARGET_cpx2 = "u-boot-ivt.sb"

BOOTLETSDIR_mxs = "BOOTLETS_DIR=${STAGING_DIR_TARGET}/boot/"
EXTRA_OEMAKE += '${BOOTLETSDIR}'

COMPATIBLE_MACHINE = "(ccardimx28js|ccimx51js|ccimx53js|cpx2)"
