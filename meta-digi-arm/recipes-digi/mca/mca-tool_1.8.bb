# Copyright (C) 2016, 2017 Digi International.

SUMMARY = "MCA firmware management tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

PKGNAME = "mca_tool"
SRC_URI = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}.tar.gz"

SRC_URI[md5sum] = "8e1c164eae31cd1d2e6d0de2fb844ae8"
SRC_URI[sha256sum] = "947545e53c64d06d6f38fc24803dc61438c3a32277849b8ff2450f2213581ac3"

S = "${WORKDIR}/${PKGNAME}-${PV}"

inherit bin_package

INSANE_SKIP_${PN} = "already-stripped"

COMPATIBLE_MACHINE = "(ccimx6ul)"
