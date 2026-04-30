# Copyright (C) 2016-2026, Digi International Inc.

SUMMARY = "MCA firmware management tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

PKGNAME = "mca_tool"

# ARM tarball
SRC_URI:arm = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=arm"
SRC_URI[arm.md5sum] = "478d700bf9e48e91dfe5901a4dfee840"
SRC_URI[arm.sha256sum] = "9d8ca0ef22c0bcfef74868f3a973615c3071259e7820ab77afa6a29f6d7b2461"

# AARCH64 tarball
SRC_URI:aarch64 = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=aarch64"
SRC_URI[aarch64.md5sum] = "a46925c03dc26b6f6337d96497d4880f"
SRC_URI[aarch64.sha256sum] = "dd20ae19dc15ca602c2951d62516c68415c9a81f9577fd893c54268a45acea3a"

S = "${WORKDIR}/${PKGNAME}-${PV}"

inherit bin_package

INSANE_SKIP:${PN} = "already-stripped"

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8m|ccimx8x|ccmp1|ccimx95)"
