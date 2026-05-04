# Copyright (C) 2016-2026, Digi International Inc.

SUMMARY = "MCA firmware management tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

PKGNAME = "mca_tool"

# ARM tarball
SRC_URI:arm = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=arm"
SRC_URI[arm.md5sum] = "f3060a3369f346b19d5932edc4caf678"
SRC_URI[arm.sha256sum] = "c8ec387c90deb441a30f0a7caa57f2f3399467e6abec1a9c50ea5b01bbad0777"

# AARCH64 tarball
SRC_URI:aarch64 = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=aarch64"
SRC_URI[aarch64.md5sum] = "bcc66113059c03e7611dc2bc83899971"
SRC_URI[aarch64.sha256sum] = "2a1ebec6cf9eebe167745df4f77b4c49b7c843c245f2de3c4873479bd40455bf"

S = "${WORKDIR}/${PKGNAME}-${PV}"

inherit bin_package

INSANE_SKIP:${PN} = "already-stripped"

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8m|ccimx8x|ccmp1|ccimx95)"
