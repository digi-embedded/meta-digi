# Copyright (C) 2024 Digi International.

SUMMARY = "Trustfence fscrypt command line tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

TF_FSCRYPT_ARCH = "${TARGET_ARCH}"
TF_FSCRYPT_ARCH:aarch64 = "arm64"

SRC_URI = "${DIGI_PKG_SRC}/${BP}-${TF_FSCRYPT_ARCH}.tar.gz;name=${TARGET_ARCH}"
SRC_URI[aarch64.md5sum] = "68291e8f9180312e5418247335434df0"
SRC_URI[aarch64.sha256sum] = "c6ffa9af67dee848e29bb10ddcbb4debd77323714e5f66f557f5ef4bf7d371f4"
SRC_URI[arm.md5sum] = "0831130450d6f0beeebbb68af9b6af29"
SRC_URI[arm.sha256sum] = "7dee4bbcff21d817bbbc152e904e8091362378446b08ad2d485f373b0da8b83b"

# Needed to resolve dependencies to libteec
RDEPENDS:${PN} += "optee-client"

inherit bin_package
