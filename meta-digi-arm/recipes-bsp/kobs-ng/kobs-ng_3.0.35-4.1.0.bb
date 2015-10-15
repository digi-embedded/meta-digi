# Copyright 2013 Digi International. All rights reserved.

SUMMARY = "Freescale's mxs nand update utility"
SECTION = "base"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=393a5ca445f6965873eca0259a17f833"

inherit autotools

SRC_URI = " \
    ${DIGI_PKG_SRC}/${PN}-${PV}.tar.gz \
    file://0001-makefile.am.patch \
    file://0002-fix-mtd-defines.patch \
    file://0003-cleanup-ROM-version-detection-code-and-add-cpx2-supp.patch \
    file://0004-discover-boot-ROM-version-from-FDT-if-available.patch \
    file://0005-dump-v1-boot-structures.patch \
    file://0006-added-option-to-verify-data-written-to-flash.patch \
    file://0007-disable-use-of-nfc_geometry.patch \
    file://0008-mtd-configure-16-bit-ECC-for-4K-page-NAND-with-224-b.patch \
"

SRC_URI[md5sum] = "2a0e55b5063605b2664fd67c95a6c686"
SRC_URI[sha256sum] = "92d2f23add8c5d3102c77f241cae26ca55871ccc613a7af833bebbbac7afb8ea"

COMPATIBLE_MACHINE = "mxs"
