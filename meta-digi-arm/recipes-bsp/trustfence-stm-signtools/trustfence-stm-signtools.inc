# Copyright (C) 2023-2026, Digi International Inc.

SUMMARY = "STM key generation and signing tools"
SECTION = "console/tools"
LICENSE = "Proprietary"

LIC_FILES_CHKSUM = "file://licenses/en.SLA0048.txt;md5=2d1dea63b940e3554570ba2e896327f4"

PKGNAME = "trustfence-stm-signtools"

# tarball
SRC_URI = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}.tar.gz"
SRC_URI[md5sum] = "2c4da5bba4c6b6f2da198248c2ba5150"
SRC_URI[sha256sum] = "aad4e073735396d4f2e39f286a6916b0c336ecd81e91545425155b2a8b56dad5"

inherit bin_package

INSANE_SKIP:${PN} = "already-stripped"
