# Copyright (C) 2016, 2017 Digi International.

SUMMARY = "MCA firmware management tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

PKGNAME = "mca_tool"
SRC_URI = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}.tar.gz"

SRC_URI[md5sum] = "20992b6b30904ee4d75d196c957077e0"
SRC_URI[sha256sum] = "49458475dd5df11f0c4f4593ffc930fbcdaa457120f73ab18587d711eb9036f6"

S = "${WORKDIR}/${PKGNAME}-${PV}"

inherit bin_package

INSANE_SKIP_${PN} = "already-stripped"

COMPATIBLE_MACHINE = "(ccimx6ul)"
