# Copyright (C) 2016-2018 Digi International.

SUMMARY = "MCA firmware management tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

PKGNAME = "mca_tool"

# ARM tarball
SRC_URI_arm = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=arm"
SRC_URI[arm.md5sum] = "bf9aff9f04118cceb2afe799ab940af2"
SRC_URI[arm.sha256sum] = "e74682cb9b0f93b1e5f2e900c1a860df250bbf8733837506df3063124413d51a"

# AARCH64 tarball
SRC_URI_aarch64 = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=aarch64"
SRC_URI[aarch64.md5sum] = "bf14a7a431353c37523149af24fb44d0"
SRC_URI[aarch64.sha256sum] = "8646837296b7d80de4ef3a5374d18aaf5a1d5b96345b6ce9e05ee982794ece24"

S = "${WORKDIR}/${PKGNAME}-${PV}"

inherit bin_package

INSANE_SKIP_${PN} = "already-stripped"

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8x)"
