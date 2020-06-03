# Copyright (C) 2016-2020 Digi International.

SUMMARY = "MCA firmware management tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

PKGNAME = "mca_tool"

# ARM tarball
SRC_URI_arm = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=arm"
SRC_URI[arm.md5sum] = "2b2ce463139be67bd9929c7ce145ae42"
SRC_URI[arm.sha256sum] = "2a241dbaad099cf2d526eff6bbfdbd7c548e4391cc6751bb9eaa22d16c3cd5e4"

# AARCH64 tarball
SRC_URI_aarch64 = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=aarch64"
SRC_URI[aarch64.md5sum] = "c5f235c29fd663fde135095016daed4f"
SRC_URI[aarch64.sha256sum] = "9a0fd1bb053d9be0ede344ba22b0811528547130ef6c78432548a27dcd8f460c"

S = "${WORKDIR}/${PKGNAME}-${PV}"

inherit bin_package

INSANE_SKIP_${PN} = "already-stripped"

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8x|ccimx8m)"
