# Copyright (C) 2023,2024, Digi International Inc.

SUMMARY = "STM key generation and signing tools"
SECTION = "console/tools"
LICENSE = "Proprietary"

LIC_FILES_CHKSUM = "file://licenses/en.SLA0048.txt;md5=108361d167deef887d204830aba9cf94"

PKGNAME = "trustfence-stm-signtools"

# tarball
SRC_URI = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}.tar.gz"
SRC_URI[md5sum] = "5693ed0cd34f858dea5bc3d1a94e9eee"
SRC_URI[sha256sum] = "be3ab77b98f5f8b8508cda090cc5eeccd0cdc39ebe9cee45b5ef52528c4ce515"

inherit bin_package

INSANE_SKIP:${PN} = "already-stripped"
