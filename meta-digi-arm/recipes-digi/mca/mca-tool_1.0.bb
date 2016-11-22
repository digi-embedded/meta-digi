# Copyright (C) 2016 Digi International.

SUMMARY = "MCA firmware management tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

PKGNAME = "mca_tool"
SRC_URI = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}.tar.gz"

SRC_URI[md5sum] = "e98ce0b9b0dcf4b3f8de5b8bf8769c7f"
SRC_URI[sha256sum] = "07b2fdc8b87376bf89e3448c9f791cf19a20861a463a9f7c038248e89f41af56"

S = "${WORKDIR}/${PKGNAME}-${PV}"

inherit bin_package

COMPATIBLE_MACHINE = "(ccimx6ul)"
