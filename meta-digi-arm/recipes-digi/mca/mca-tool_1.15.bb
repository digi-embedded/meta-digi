# Copyright (C) 2016-2018 Digi International.

SUMMARY = "MCA firmware management tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

PKGNAME = "mca_tool"

# ARM tarball
SRC_URI_arm = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=arm"
SRC_URI[arm.md5sum] = "8069da600af8b607c96d1edf9c5dad8c"
SRC_URI[arm.sha256sum] = "54a13f497161106c785e81e8dfe506ce46c6c1ecf3e81185055112c9217506d3"

# AARCH64 tarball
SRC_URI_aarch64 = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=aarch64"
SRC_URI[aarch64.md5sum] = "ee0b11428450b48b8ad3f27d3ebe0556"
SRC_URI[aarch64.sha256sum] = "032691491e2b5f294992fab453f2864666b3af495b439be897f23adacb312827"

S = "${WORKDIR}/${PKGNAME}-${PV}"

inherit bin_package

INSANE_SKIP_${PN} = "already-stripped"

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8x)"
