# Copyright (C) 2016-2018 Digi International.

SUMMARY = "MCA firmware management tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

PKGNAME = "mca_tool"

# ARM tarball
SRC_URI_arm = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=arm"
SRC_URI[arm.md5sum] = "a9c9e4bbafa65b8140def4db60518a61"
SRC_URI[arm.sha256sum] = "70bae34b602573547cd6f9c6a738aaef4f90c03ce7a97b4d54013b20acbf9a45"

# AARCH64 tarball
SRC_URI_aarch64 = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=aarch64"
SRC_URI[aarch64.md5sum] = "ccde3a7d4981e179b271a3ce258c036c"
SRC_URI[aarch64.sha256sum] = "da6e9710dd4ff07451cbd1e6f7961f42e97a1c6c0f8b25141be48b229aa9b5a8"

S = "${WORKDIR}/${PKGNAME}-${PV}"

inherit bin_package

INSANE_SKIP_${PN} = "already-stripped"

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8x)"
