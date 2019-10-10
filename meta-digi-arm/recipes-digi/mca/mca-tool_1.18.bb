# Copyright (C) 2016-2019 Digi International.

SUMMARY = "MCA firmware management tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

PKGNAME = "mca_tool"

# ARM tarball
SRC_URI_arm = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=arm"
SRC_URI[arm.md5sum] = "aa2037b93dadafe160824f7d63dd8582"
SRC_URI[arm.sha256sum] = "7dbc07672d5cce52e3ec611a1bcef1b6c0aed278fbcd6a150b3f528d8da66065"

# AARCH64 tarball
SRC_URI_aarch64 = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=aarch64"
SRC_URI[aarch64.md5sum] = "40d8113353daf8e426597c289f6659d8"
SRC_URI[aarch64.sha256sum] = "a0f7d8bae72f9dcdb1102bcf3e42a4b66d4f66b4bef43f80a4fd63282562a9d4"

S = "${WORKDIR}/${PKGNAME}-${PV}"

inherit bin_package

INSANE_SKIP_${PN} = "already-stripped"

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8x)"
