# Copyright (C) 2023,2025, Digi International Inc.

SUMMARY = "STM key generation and signing tools"
SECTION = "console/tools"
LICENSE = "Proprietary"

LIC_FILES_CHKSUM = "file://licenses/en.SLA0048.txt;md5=108361d167deef887d204830aba9cf94"

PKGNAME = "trustfence-stm-signtools"

# tarball
SRC_URI = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}.tar.gz"
SRC_URI[md5sum] = "a9277220a763b85efac2506dc2ad16f9"
SRC_URI[sha256sum] = "b45ceff90fde83f3e536a84bd7d2ae5ca95d695f35edbdab148f78d3cc31ce5c"

inherit bin_package

INSANE_SKIP:${PN} = "already-stripped"
