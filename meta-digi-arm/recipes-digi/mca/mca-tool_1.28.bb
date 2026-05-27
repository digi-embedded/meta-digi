# Copyright (C) 2016-2026, Digi International Inc.

SUMMARY = "MCA firmware management tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

PKGNAME = "mca_tool"

# ARM tarball
SRC_URI:arm = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=arm"
SRC_URI[arm.md5sum] = "81cd52b2af1ec80255cc11871731777a"
SRC_URI[arm.sha256sum] = "c10bc5ba3bc31b5278531e145f766c9ac34a4123bde726b1939f5b0164de96cb"

# AARCH64 tarball
SRC_URI:aarch64 = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=aarch64"
SRC_URI[aarch64.md5sum] = "9a79e7ed6efc9c08c64bee8db94009a1"
SRC_URI[aarch64.sha256sum] = "2e587b518898451f431f4c0acbdd1c130798a16a57a611c075513cecff901703"

S = "${WORKDIR}/${PKGNAME}-${PV}"

inherit bin_package

INSANE_SKIP:${PN} = "already-stripped"

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8m|ccimx8x|ccmp1|ccimx95)"
