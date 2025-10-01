# Copyright (C) 2023,2025, Digi International Inc.

SUMMARY = "STM key generation and signing tools"
SECTION = "console/tools"
LICENSE = "Proprietary"

LIC_FILES_CHKSUM = "file://licenses/en.SLA0048.txt;md5=45cb993af0ee0a0df02181b29e244eec"

PKGNAME = "trustfence-stm-signtools"

# tarball
SRC_URI = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}.tar.gz"
SRC_URI[md5sum] = "2865f224d9ed3e3d3f3ad4f1a96c1ec8"
SRC_URI[sha256sum] = "e7a862e1c6847906d11de67fa7d4ba9efd2448094b41cf3dccb78300dcd839df"

inherit bin_package

INSANE_SKIP:${PN} = "already-stripped"
