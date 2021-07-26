# Copyright (C) 2016-2021 Digi International.

SUMMARY = "MCA firmware management tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

PKGNAME = "mca_tool"

# ARM tarball
SRC_URI_arm = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=arm"
SRC_URI[arm.md5sum] = "bbeded0a955a026d302cd4ee212d920e"
SRC_URI[arm.sha256sum] = "33de60f59bfa3e4b867bd872f05413a823852a1e33b21efcddd714454978bb9a"

# AARCH64 tarball
SRC_URI_aarch64 = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=aarch64"
SRC_URI[aarch64.md5sum] = "cf64de7f5aad9cd6a102afd18aebd8bb"
SRC_URI[aarch64.sha256sum] = "c4652b8c0dd54315d7890c47798199948ea595a7dea8de5b6d68a1bfd3853557"

S = "${WORKDIR}/${PKGNAME}-${PV}"

inherit bin_package

INSANE_SKIP_${PN} = "already-stripped"

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8x|ccimx8m)"
