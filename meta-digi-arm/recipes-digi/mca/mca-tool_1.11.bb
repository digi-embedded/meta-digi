# Copyright (C) 2016, 2017 Digi International.

SUMMARY = "MCA firmware management tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

PKGNAME = "mca_tool"
SRC_URI = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}.tar.gz"

SRC_URI[md5sum] = "7a2c1119914cfc59c2b8463dba155d03"
SRC_URI[sha256sum] = "71a9002851520947aca443b536327b08a5f413e71a9d6ba79d00e32180220b71"

S = "${WORKDIR}/${PKGNAME}-${PV}"

inherit bin_package

INSANE_SKIP_${PN} = "already-stripped"

COMPATIBLE_MACHINE = "(ccimx6ul)"
