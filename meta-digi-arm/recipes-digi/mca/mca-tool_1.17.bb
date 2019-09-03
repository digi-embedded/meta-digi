# Copyright (C) 2016-2019 Digi International.

SUMMARY = "MCA firmware management tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

PKGNAME = "mca_tool"

# ARM tarball
SRC_URI_arm = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=arm"
SRC_URI[arm.md5sum] = "c1eefe3113c4915b92fadebc7f769d21"
SRC_URI[arm.sha256sum] = "9a9d962e549fdb0f22fc1037f74ca21c7356dded7033c28a1b4b325a44b579aa"

# AARCH64 tarball
SRC_URI_aarch64 = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-${TUNE_ARCH}.tar.gz;name=aarch64"
SRC_URI[aarch64.md5sum] = "92a3d3d3a63c749efc4761cdb5efe77e"
SRC_URI[aarch64.sha256sum] = "6b3cd2e9aa879ebd5aba628731855112db69ab01cedc5e203c10917e51f93a08"

S = "${WORKDIR}/${PKGNAME}-${PV}"

inherit bin_package

INSANE_SKIP_${PN} = "already-stripped"

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8x)"
