# Copyright (C) 2016-2018 Digi International.

SUMMARY = "MCA firmware management tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

PKGNAME = "mca_tool"

# ARM tarball
SRC_URI_arm = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=arm"
SRC_URI[arm.md5sum] = "5ed2579b9e3e5bcb2bc10a8082f8be2b"
SRC_URI[arm.sha256sum] = "cfdb464893e02e37f5baaf9f08ac0fb8943a9a70a8fea6737d1b65c659ed9c42"

# AARCH64 tarball
SRC_URI_aarch64 = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=aarch64"
SRC_URI[aarch64.md5sum] = "9b6c4768205dda0444e04431245881d5"
SRC_URI[aarch64.sha256sum] = "a5e5947d62a6199afeeff74d48eb5b1c08a7f8836c84468b41e9f51773b54122"

S = "${WORKDIR}/${PKGNAME}-${PV}"

inherit bin_package

INSANE_SKIP_${PN} = "already-stripped"

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8x)"
