# Copyright (C) 2016-2021, Digi International Inc.

SUMMARY = "Trustfence command line tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

SRC_URI:arm = "${DIGI_PKG_SRC}/${BP}-${TUNE_ARCH}.tar.gz;name=arm"

SRC_URI[arm.md5sum] = "2afde78b6c84aac1fbec2dbce9e49aab"
SRC_URI[arm.sha256sum] = "94a0158aa9e09f8d13be8630e1426fe9a1fdb4b4a2bb133be2f7726a3a27f9d7"

SRC_URI:aarch64 = "${DIGI_PKG_SRC}/${BP}-${TUNE_ARCH}.tar.gz;name=aarch64"

SRC_URI[aarch64.md5sum] = "a9b4a53eaf493a6cd03090fead217b38"
SRC_URI[aarch64.sha256sum] = "a134e65e305e19bf5a3734b1e095f4bb8fbe80482cbad02f3ce05f936a9c3801"

inherit bin_package
