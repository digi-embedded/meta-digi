# Copyright (C) 2016-2020 Digi International.

SUMMARY = "Trustfence command line tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

SRC_URI_arm = "${DIGI_PKG_SRC}/${BP}-${TUNE_ARCH}.tar.gz;name=arm"

SRC_URI[arm.md5sum] = "926c31fecec8e28a6ed30984b19e868f"
SRC_URI[arm.sha256sum] = "1ec9cae98553e9917ff1a88bfce17fce749a4a8af28b8c40e24c8eebb7540faa"

SRC_URI_aarch64 = "${DIGI_PKG_SRC}/${BP}-${TUNE_ARCH}.tar.gz;name=aarch64"

SRC_URI[aarch64.md5sum] = "eec9ff6c3b715ec37c8a38997f446581"
SRC_URI[aarch64.sha256sum] = "fa738cce350028d74363c78fdca567263c4863389d3741e9f8761486d97e99a6"

inherit bin_package
