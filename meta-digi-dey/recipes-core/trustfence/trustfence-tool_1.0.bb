# Copyright (C) 2016 Digi International.

SUMMARY = "Trustfence command line tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

SRC_URI = "${DIGI_PKG_SRC}/${BP}.tar.gz"

SRC_URI[md5sum] = "aa9b8b530402f412886e1f8f1da466e6"
SRC_URI[sha256sum] = "dbeb718158a4a55552bb3a2c03990df3167331bb772925f824ebc02348e29089"

inherit bin_package
