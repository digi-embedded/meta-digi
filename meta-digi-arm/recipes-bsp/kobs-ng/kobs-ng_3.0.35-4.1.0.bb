# Copyright 2013 Digi International. All rights reserved.

SUMMARY = "Freescale's mxs nand update utility"
SECTION = "base"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=393a5ca445f6965873eca0259a17f833"

PR = "${DISTRO}.r0"

inherit autotools

SRC_URI = " \
    ${DIGI_MIRROR}/${PN}-${PV}.tar.gz \
    file://0001-makefile.am.patch \
    file://0002-fix-mtd-defines.patch \
"

SRC_URI[md5sum] = "2a0e55b5063605b2664fd67c95a6c686"
SRC_URI[sha256sum] = "92d2f23add8c5d3102c77f241cae26ca55871ccc613a7af833bebbbac7afb8ea"

COMPATIBLE_MACHINE = "mxs"
