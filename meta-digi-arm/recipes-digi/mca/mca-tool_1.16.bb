# Copyright (C) 2016-2019 Digi International.

SUMMARY = "MCA firmware management tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

PKGNAME = "mca_tool"

# ARM tarball
SRC_URI_arm = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=arm"
SRC_URI[arm.md5sum] = "d6043cd754a8ea0449a4b1afc80b9775"
SRC_URI[arm.sha256sum] = "4219efae7d3b327bac7940b992a446cdbf4cd3ed25d3234e1ef63ed32b6db595"

# AARCH64 tarball
SRC_URI_aarch64 = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=aarch64"
SRC_URI[aarch64.md5sum] = "937f910b33aa9f6fe0d24317836fd620"
SRC_URI[aarch64.sha256sum] = "225c64e07c7158e0849cdd730d3c073cc87089a60b42d4e86a82c882761adb6c"

S = "${WORKDIR}/${PKGNAME}-${PV}"

inherit bin_package

INSANE_SKIP_${PN} = "already-stripped"

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8x)"
