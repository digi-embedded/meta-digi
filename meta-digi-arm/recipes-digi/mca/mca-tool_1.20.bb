# Copyright (C) 2016-2020 Digi International.

SUMMARY = "MCA firmware management tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

PKGNAME = "mca_tool"

# ARM tarball
SRC_URI_arm = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=arm"
SRC_URI[arm.md5sum] = "2007a858cd8f82ae2ddbff43f437335a"
SRC_URI[arm.sha256sum] = "d9a0c1ca1c9f20041602edd585c31bd79b75bc8d8a74648d1809350e731af3d5"

# AARCH64 tarball
SRC_URI_aarch64 = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=aarch64"
SRC_URI[aarch64.md5sum] = "600451f7312a130c63f3b3b5c7abdbb6"
SRC_URI[aarch64.sha256sum] = "1acc7498943662e7e5bd26a67d2733a4814823170ba0756313459cb0e68e6e5f"

S = "${WORKDIR}/${PKGNAME}-${PV}"

inherit bin_package

INSANE_SKIP_${PN} = "already-stripped"

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8x|ccimx8m)"
