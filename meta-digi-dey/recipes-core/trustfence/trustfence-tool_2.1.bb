# Copyright (C) 2016-2018 Digi International.

SUMMARY = "Trustfence command line tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

SRC_URI_arm = "${DIGI_PKG_SRC}/${BP}-${TUNE_ARCH}.tar.gz;name=arm"

SRC_URI[arm.md5sum] = "31385122bed83d420f92efddd8975e99"
SRC_URI[arm.sha256sum] = "e5e9157837be8e26141708e06a881ef872dd94aa06451668959845c4d4d19efc"

SRC_URI_aarch64 = "${DIGI_PKG_SRC}/${BP}-${TUNE_ARCH}.tar.gz;name=aarch64"

SRC_URI[aarch64.md5sum] = "41116c3d1f5a71f7f6d97571ad52b872"
SRC_URI[aarch64.sha256sum] = "3fe7d39140b1b73d001afd220bf83965116175022f3a9f42695a752c23637e04"

inherit bin_package
