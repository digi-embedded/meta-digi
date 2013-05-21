# Copyright 2013 Digi International. All rights reserved.

SUMMARY = "Freescale's mxs nand update utility"
SECTION = "base"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

PR = "${DISTRO}.r0"

SRC_URI = "${DIGI_MIRROR}/${PN}-${PV}.tar.gz \
           file://0001-dump-v1-boot-structures.patch \
           file://0002-rom-version.patch \
           file://0003-ncb-fixed-transposed-parameters-in-memset.patch \
           file://0004-added-verification-of-written-data-only-for-v1-ROM.patch \
          "

SRC_URI_append_ccardimx28js += "file://0005-version-parse-MX-arch-to-select-rom-version.patch"

SRC_URI[md5sum] = "9fce401b6c90e851f0335b9ca3a649a9"
SRC_URI[sha256sum] = "ef25f5c9033500c236b1894436bddc4e20b90bc17585fbcdf9fe3bbbd9f15781"

inherit autotools

COMPATIBLE_MACHINE = "(ccardimx28js|cpx2|wr21)"

BBCLASSEXTEND = "native"

