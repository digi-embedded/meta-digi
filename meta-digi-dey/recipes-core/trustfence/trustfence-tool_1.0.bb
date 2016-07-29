# Copyright (C) 2016 Digi International.

SUMMARY = "Trustfence command line tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

SRC_URI = "${DIGI_PKG_SRC}/${BP}.tar.gz"

SRC_URI[md5sum] = "eea4efe8b8e7527a0ffeea16fd238ba3"
SRC_URI[sha256sum] = "aefeb08f2db59c891cf1162488499448bf9d80d64b2778d4fda11343793373e7"

inherit bin_package
