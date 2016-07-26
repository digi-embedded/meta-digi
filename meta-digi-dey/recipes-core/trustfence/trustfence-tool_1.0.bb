# Copyright (C) 2016 Digi International.

SUMMARY = "Trustfence command line tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

SRC_URI = "${DIGI_PKG_SRC}/${BP}.tar.gz"

SRC_URI[md5sum] = "fb1e9b69862f8ff46d6de988b19cf8f9"
SRC_URI[sha256sum] = "bd36764ff424b72b676d0c5b71456827ed13c55f64c309c81bfd527e7bee2b7f"

inherit bin_package
