# Copyright (C) 2026, Digi International Inc.

SUMMARY = "Cinterion PLSx3 firmware update tool (glinswup) + helper script"
SECTION = "console/tools"
LICENSE = "CLOSED"

PKGNAME = "glinswup_PLSx3"

# ARM tarball
SRC_URI:arm += "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-arm.tar.gz;name=arm"
SRC_URI[arm.md5sum] = "e89f7e3e9cc97f4df51366d4a93e7542"
SRC_URI[arm.sha256sum] = "ca274365c244c3b43ce3e2e3b7b13c70a4104645673c7324a4cbc18132e28a86"

# AARCH64 tarball
SRC_URI:aarch64 += "${DIGI_PKG_SRC}/${PKGNAME}-${PV}-aarch64.tar.gz;name=aarch64"
SRC_URI[aarch64.md5sum] = "4e44cd327ce7fe430e1d60d66ae7528e"
SRC_URI[aarch64.sha256sum] = "3f70500a4c6c280c3c42259fa51abc565fb76c97d4cea64aed36a3539b168c38"

SRC_URI += " file://pls3_fw_update.sh"

S = "${WORKDIR}/${PKGNAME}_${PV}"

do_install:append() {
	install -d ${D}${sbindir}
	install -m 0755 ${WORKDIR}/pls3_fw_update.sh ${D}${sbindir}/pls3_fw_update.sh
}

inherit bin_package

INSANE_SKIP:${PN} = "already-stripped"
