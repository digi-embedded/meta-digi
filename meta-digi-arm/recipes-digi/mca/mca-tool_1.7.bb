# Copyright (C) 2016, 2017 Digi International.

SUMMARY = "MCA firmware management tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

PKGNAME = "mca_tool"
SRC_URI = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}.tar.gz"

SRC_URI[md5sum] = "b4159e354a530f1cabc4c98cbd30c2e5"
SRC_URI[sha256sum] = "e412e6c815731560e7e32fd2ae86b0ee946735c2ba3e146be30dd07c90518330"

S = "${WORKDIR}/${PKGNAME}-${PV}"

inherit bin_package

INSANE_SKIP_${PN} = "already-stripped"

COMPATIBLE_MACHINE = "(ccimx6ul)"
