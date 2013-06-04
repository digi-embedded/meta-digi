# Copyright (C) 2012 Digi International

DESCRIPTION = "Bootloader for Digi platforms"
require recipes-bsp/u-boot/u-boot.inc

PROVIDES += "u-boot"

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://COPYING;md5=4c6cde5df68eff615d36789dc18edd3b"

PR = "r0"

# Uncomment to build the from sources (internal use only)
# UBOOT_BUILD_SRC ?= "1"

SRCREV = "4af0b5f73215c6f075e17f866d831a948d777a2a"
SRCREV_SHORT = "${@'${SRCREV}'[:7]}"

# Checksums for 'u-boot-denx-${SRCREV_SHORT}.tar.gz' tarballs
TARBALL_MD5    = "73da801842acd3f8ac7200c13e005c89"
TARBALL_SHA256 = "f86e05317eadf3e868da3b301e0e6e95b6fdb9daebed4cb3f8494809fbe31b03"

SRC_URI_git = "${DIGI_LOG_GIT}u-boot-denx.git;protocol=git;branch=refs/heads/master"
SRC_URI_tarball = " \
        ${DIGI_MIRROR}/u-boot-denx-${SRCREV_SHORT}.tar.gz;md5sum=${TARBALL_MD5};sha256sum=${TARBALL_SHA256} \
        "

SRC_URI  = "${@base_conditional('UBOOT_BUILD_SRC', '1' , '${SRC_URI_git}', '${SRC_URI_tarball}', d)}"

S  = "${@base_conditional('UBOOT_BUILD_SRC', '1' , '${WORKDIR}/git', '${WORKDIR}/u-boot-denx-${SRCREV_SHORT}', d)}"
EXTRA_OEMAKE += 'HOSTSTRIP=true'

DEPENDS_mxs += "elftosb-native imx-bootlets-del"

BOOTLETSDIR_mxs = "BOOTLETS_DIR=${STAGING_DIR_TARGET}/boot/"
EXTRA_OEMAKE += '${BOOTLETSDIR}'

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccardimx28js|ccimx51js|ccimx53js|cpx2|wr21)"
