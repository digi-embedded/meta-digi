# Copyright (C) 2016-2022 Digi International.

SUMMARY = "MCA firmware management tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

PKGNAME = "mca_tool"

# ARM tarball
SRC_URI:arm = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=arm"
SRC_URI[arm.md5sum] = "ffa8967cb9b684f3846b641a5d57b8f6"
SRC_URI[arm.sha256sum] = "057c289990d79f0b749e9d0d7af2570332e9215e697de75dc6851d89bdd61dff"

# AARCH64 tarball
SRC_URI:aarch64 = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=aarch64"
SRC_URI[aarch64.md5sum] = "12033830965f2861628461c612a7604e"
SRC_URI[aarch64.sha256sum] = "2467e426c6a4e6b89f4aaced846c1f52787e130f16ffb62e6f046bea7bc4f21f"

S = "${WORKDIR}/${PKGNAME}-${PV}"

inherit bin_package

INSANE_SKIP:${PN} = "already-stripped"

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8x|ccimx8m|ccmp15)"
