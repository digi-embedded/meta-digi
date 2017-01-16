# Copyright (C) 2016 Digi International.

SUMMARY = "MCA firmware management tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

PKGNAME = "mca_tool"
SRC_URI = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}.tar.gz"

SRC_URI[md5sum] = "a08dd404f7c5165b6949ae09117a27d3"
SRC_URI[sha256sum] = "6978e481cbd936f03e93f3c566cc506570ebae82b3b499a79da65c3c2d320191"

S = "${WORKDIR}/${PKGNAME}-${PV}"

inherit bin_package

COMPATIBLE_MACHINE = "(ccimx6ul)"
