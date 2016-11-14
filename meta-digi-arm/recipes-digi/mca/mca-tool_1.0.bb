# Copyright (C) 2016 Digi International.

SUMMARY = "MCA firmware management tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

PKGNAME = "mca_tool"
SRC_URI = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}.tar.gz"

SRC_URI[md5sum] = "ea869665e49f5c964bfb4ebc08c0f123"
SRC_URI[sha256sum] = "d0fecdeac90b94b4399418a2e64e44f4ac2786abf66aeed8d3dab953e74d1ae4"

S = "${WORKDIR}/${PKGNAME}-${PV}"

inherit bin_package

COMPATIBLE_MACHINE = "(ccimx6ul)"
