# Copyright (C) 2016, 2017 Digi International.

SUMMARY = "Trustfence command line tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

SRC_URI = "${DIGI_PKG_SRC}/${BP}.tar.gz"

SRC_URI[md5sum] = "b26c0b8a4fd819beba4d0af6b11bc603"
SRC_URI[sha256sum] = "7dc772e40e76d94ac297098c7fcfb19254994afb4c6ee30d23fb056a872e6543"

inherit bin_package
