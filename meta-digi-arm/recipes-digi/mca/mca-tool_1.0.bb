# Copyright (C) 2016 Digi International.

SUMMARY = "MCA firmware management tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

PKGNAME = "mca_tool"
SRC_URI = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}.tar.gz"

SRC_URI[md5sum] = "bee011ed0f1f2394c55bedaeda92f5cf"
SRC_URI[sha256sum] = "d6983fb0941a8b8c63c7369ef7285190be19ef446fc2a206e25a2c545a1ea671"

S = "${WORKDIR}/${PKGNAME}-${PV}"

inherit bin_package

COMPATIBLE_MACHINE = "(ccimx6ul)"
