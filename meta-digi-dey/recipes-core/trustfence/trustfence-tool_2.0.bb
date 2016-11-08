# Copyright (C) 2016 Digi International.

SUMMARY = "Trustfence command line tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

SRC_URI = "${DIGI_PKG_SRC}/${BP}.tar.gz"

SRC_URI[md5sum] = "413084cc2045d345883189cd0d68ca76"
SRC_URI[sha256sum] = "dff702f2838a7802103469c1ba07daead206652774e02a0a855b08d94aafe5fe"

inherit bin_package
