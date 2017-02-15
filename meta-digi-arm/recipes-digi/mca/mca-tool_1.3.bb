# Copyright (C) 2016 Digi International.

SUMMARY = "MCA firmware management tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

PKGNAME = "mca_tool"
SRC_URI = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}.tar.gz"

SRC_URI[md5sum] = "d54692090bf0820e6116364822f6433f"
SRC_URI[sha256sum] = "e8267e66ba496a1e77b2dec70d6555bdeb8e06b267e776c86b9d4b01e9d13eb5"

S = "${WORKDIR}/${PKGNAME}-${PV}"

inherit bin_package

COMPATIBLE_MACHINE = "(ccimx6ul)"
