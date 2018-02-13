# Copyright (C) 2016-2018 Digi International.

SUMMARY = "MCA firmware management tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

PKGNAME = "mca_tool"
SRC_URI = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}.tar.gz"

SRC_URI[md5sum] = "bf9aff9f04118cceb2afe799ab940af2"
SRC_URI[sha256sum] = "e74682cb9b0f93b1e5f2e900c1a860df250bbf8733837506df3063124413d51a"

S = "${WORKDIR}/${PKGNAME}-${PV}"

inherit bin_package

INSANE_SKIP_${PN} = "already-stripped"

COMPATIBLE_MACHINE = "(ccimx6ul)"
