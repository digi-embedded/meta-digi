# Copyright (C) 2023,2024 Digi International.

SUMMARY = "STM key generation and signing tools"
SECTION = "console/tools"
LICENSE = "Proprietary"

LIC_FILES_CHKSUM = "file://licenses/en.SLA0048.txt;md5=108361d167deef887d204830aba9cf94"

PKGNAME = "trustfence-stm-signtools"

# tarball
SRC_URI = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}.tar.gz"
SRC_URI[md5sum] = "995b612c1da7e3d9b9b402ede578f41d"
SRC_URI[sha256sum] = "b3c975c12f71acda356e086c1440398ca95256e104fd4813ef6a720237f6dd72"

inherit bin_package

INSANE_SKIP:${PN} = "already-stripped"
