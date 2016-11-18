# Copyright (C) 2016 Digi International.

SUMMARY = "Trustfence command line tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

SRC_URI = "${DIGI_PKG_SRC}/${BP}.tar.gz"

SRC_URI[md5sum] = "9556aec9c9b0ef7e38606040e4f059d0"
SRC_URI[sha256sum] = "21f013616393883a5c3e0e9d7332e5d169af515f83b3a7dbf365e9ffbde1c593"

inherit bin_package
