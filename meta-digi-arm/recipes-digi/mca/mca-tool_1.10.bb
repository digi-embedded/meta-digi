# Copyright (C) 2016, 2017 Digi International.

SUMMARY = "MCA firmware management tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

PKGNAME = "mca_tool"
SRC_URI = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}.tar.gz"

SRC_URI[md5sum] = "e739879489b2d0f0ab2fa61f60af80f6"
SRC_URI[sha256sum] = "af2eb7abebfbabe228c574b887d166d2ef5ad5b3a9308ccd07778d0ccbed1e8b"

S = "${WORKDIR}/${PKGNAME}-${PV}"

inherit bin_package

INSANE_SKIP_${PN} = "already-stripped"

COMPATIBLE_MACHINE = "(ccimx6ul)"
