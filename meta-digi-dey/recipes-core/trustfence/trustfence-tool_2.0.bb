# Copyright (C) 2016 Digi International.

SUMMARY = "Trustfence command line tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

SRC_URI = "${DIGI_PKG_SRC}/${BP}.tar.gz"

SRC_URI[md5sum] = "dfbe0f7a5c2d16c5abafae14eb33d592"
SRC_URI[sha256sum] = "e335fc7080fb35ad198319c06ec31c77d9d2fe63219b9adc1ffc1e686e1534ae"

inherit bin_package
