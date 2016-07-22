# Copyright (C) 2016 Digi International.

SUMMARY = "Trustfence command line tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

SRC_URI = "${DIGI_PKG_SRC}/${BP}.tar.gz"

SRC_URI[md5sum] = "1140b71d0e619001b677117e8938be48"
SRC_URI[sha256sum] = "13eecca139dfb6470204c75291c5791144dea098653f52d39d847b2aee3fe19b"

inherit bin_package
