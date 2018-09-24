# Copyright (C) 2016-2018 Digi International.

SUMMARY = "Trustfence command line tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

SRC_URI_arm = "${DIGI_PKG_SRC}/${BP}-${TUNE_ARCH}.tar.gz;name=arm"

SRC_URI[arm.md5sum] = "443fe53304c2c3021150abc4dd7cf5d2"
SRC_URI[arm.sha256sum] = "d52b0ecd11d69a88341b01c1fdf9789500ef8ef9b0e8e12747aa85ded4d0b315"

SRC_URI_aarch64 = "${DIGI_PKG_SRC}/${BP}-${TUNE_ARCH}.tar.gz;name=aarch64"

SRC_URI[aarch64.md5sum] = "4608cc2594ac222578575e5ca4aecc1c"
SRC_URI[aarch64.sha256sum] = "f599e8627f798dcb43254ec52200d4d406662ad0fb79b95e57f75da870828625"

inherit bin_package
