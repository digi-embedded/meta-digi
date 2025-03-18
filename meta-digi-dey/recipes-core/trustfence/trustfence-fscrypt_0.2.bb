# Copyright (C) 2024,2025 Digi International Inc.

SUMMARY = "Trustfence fscrypt command line tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

TF_FSCRYPT_ARCH = "${TARGET_ARCH}"
TF_FSCRYPT_ARCH:aarch64 = "arm64"

SRC_URI = "${DIGI_PKG_SRC}/${BP}-${TF_FSCRYPT_ARCH}.tar.gz;name=${TARGET_ARCH}"
SRC_URI[aarch64.md5sum] = "8a125d98762cd3d51097699a7718f280"
SRC_URI[aarch64.sha256sum] = "6c7846389f1b30d12b9a8f08bf09db3ed44532c61625d67d1b2334496228099d"
SRC_URI[arm.md5sum] = "0831130450d6f0beeebbb68af9b6af29"
SRC_URI[arm.sha256sum] = "7dee4bbcff21d817bbbc152e904e8091362378446b08ad2d485f373b0da8b83b"

# Trustfence fscrypt based on libteec v1.0.0
SRC_URI:stm32mpcommon = "${DIGI_PKG_SRC}/${BP}-${TF_FSCRYPT_ARCH}-libteec-v1.0.0.tar.gz;name=${TARGET_ARCH}-libteecv1"
SRC_URI[aarch64-libteecv1.md5sum] = "538f3bff6c9cd6110b7207581e4300d3"
SRC_URI[aarch64-libteecv1.sha256sum] = "a15a657f7350f9f5c9d39eb0f5d634174c1bc22d7e50333cc01de295e95c0fa4"

# Needed to resolve dependencies to libteec
RDEPENDS:${PN} += "optee-client"

inherit bin_package

INSANE_SKIP:${PN} = "already-stripped"
