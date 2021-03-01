# Copyright (C) 2016-2021 Digi International.

SUMMARY = "MCA firmware management tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

PKGNAME = "mca_tool"

# ARM tarball
SRC_URI_arm = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=arm"
SRC_URI[arm.md5sum] = "eeb242ee2baeda8a9a1a48dea634b6ea"
SRC_URI[arm.sha256sum] = "40794e0c3baad39b04f2d1640c29620a08036fb36b18583d4f7828fe860bcd89"

# AARCH64 tarball
SRC_URI_aarch64 = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=aarch64"
SRC_URI[aarch64.md5sum] = "bd90f295095fead2e58bfa2f6b995c8f"
SRC_URI[aarch64.sha256sum] = "64dd676dff0ffee143d22268e0512b40fe428ecf8b5e31496f738fb9b1a8a01f"

S = "${WORKDIR}/${PKGNAME}-${PV}"

inherit bin_package

INSANE_SKIP_${PN} = "already-stripped"

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8x|ccimx8m)"
