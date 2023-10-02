# Copyright (C) 2023 Digi International.

SUMMARY = "STM key generation and signing tools"
SECTION = "console/tools"
LICENSE = "Proprietary"

LIC_FILES_CHKSUM = "file://en.SLA0048.txt;md5=108361d167deef887d204830aba9cf94"

PKGNAME = "trustfence-stm-signtools"

# tarball
SRC_URI = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}.tar.gz"
SRC_URI[md5sum] = "38ce0cd682350b15e773e725ee842da4"
SRC_URI[sha256sum] = "cc349bdcaa764d5af25b12ee9e0c143f93dc80d531f42f8a57a735e170cd378b"

inherit bin_package

INSANE_SKIP:${PN} = "already-stripped"
