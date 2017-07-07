# Copyright (C) 2016, 2017 Digi International.

SUMMARY = "Trustfence command line tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

SRC_URI = "${DIGI_PKG_SRC}/${BP}.tar.gz"

SRC_URI[md5sum] = "31385122bed83d420f92efddd8975e99"
SRC_URI[sha256sum] = "e5e9157837be8e26141708e06a881ef872dd94aa06451668959845c4d4d19efc"

inherit bin_package
