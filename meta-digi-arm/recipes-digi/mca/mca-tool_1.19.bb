# Copyright (C) 2016-2020 Digi International.

SUMMARY = "MCA firmware management tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

PKGNAME = "mca_tool"

# ARM tarball
SRC_URI_arm = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=arm"
SRC_URI[arm.md5sum] = "f677ccb2b6b627838c028f2281aa5b8c"
SRC_URI[arm.sha256sum] = "180ad5cdb9ddce7980db8019e9e710fcabde39ed896614f3ec8a05fedeb479f1"

# AARCH64 tarball
SRC_URI_aarch64 = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=aarch64"
SRC_URI[aarch64.md5sum] = "183966e276383585048b85bfdf38fb49"
SRC_URI[aarch64.sha256sum] = "e7d0f375bd50b2f1936ba4770e64e8dc8f14af39cee4a85e26e4a22843a5638a"

S = "${WORKDIR}/${PKGNAME}-${PV}"

inherit bin_package

INSANE_SKIP_${PN} = "already-stripped"

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8x|ccimx8m)"
