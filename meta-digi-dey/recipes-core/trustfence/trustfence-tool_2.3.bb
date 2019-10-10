# Copyright (C) 2016-2019 Digi International.

SUMMARY = "Trustfence command line tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

SRC_URI_arm = "${DIGI_PKG_SRC}/${BP}-${TUNE_ARCH}.tar.gz;name=arm"

SRC_URI[arm.md5sum] = "ef253e2c781d76820f38832ceec22ada"
SRC_URI[arm.sha256sum] = "5fc478f76848b438cc28cbdc7f8eabd63f7db4b2998a5a46d75602a80365a596"

SRC_URI_aarch64 = "${DIGI_PKG_SRC}/${BP}-${TUNE_ARCH}.tar.gz;name=aarch64"

SRC_URI[aarch64.md5sum] = "a109efb8a2c7b57f2e83136ada24938d"
SRC_URI[aarch64.sha256sum] = "ddb9cb94b4ffa182e479cfbcc0f57774a6c68ac813b4af747aa493c259970c5b"

inherit bin_package
