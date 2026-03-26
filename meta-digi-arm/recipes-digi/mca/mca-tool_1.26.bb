# Copyright (C) 2016-2026, Digi International Inc.

SUMMARY = "MCA firmware management tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

PKGNAME = "mca_tool"

# ARM tarball
SRC_URI:arm = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=arm"
SRC_URI[arm.md5sum] = "8f3aba8180fd5408e4c2c631462fdf6f"
SRC_URI[arm.sha256sum] = "ac07ea5dfaceb96b08ab09c41346bcdc2d6564da5a43c9302f9607147da85225"

# AARCH64 tarball
SRC_URI:aarch64 = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=aarch64"
SRC_URI[aarch64.md5sum] = "1e33064458b5a2b0ad250e0e88ac7157"
SRC_URI[aarch64.sha256sum] = "2d677b3aa5c2bc16db3ebd03f4636b50dfbdb7973f7680d725b4bb1ca785ba6b"

S = "${WORKDIR}/${PKGNAME}-${PV}"

inherit bin_package

INSANE_SKIP:${PN} = "already-stripped"

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8m|ccimx8x|ccmp1|ccimx95)"
