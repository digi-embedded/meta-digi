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
SRC_URI[aarch64-libteecv1.md5sum] = "67c2d87312ccae750de8165e82691c63"
SRC_URI[aarch64-libteecv1.sha256sum] = "43c2e900ca8d0aaac15963ffb5a7c57e3dd07613b2fe236955012039027b03f4"
SRC_URI[arm-libteecv1.md5sum] = "6b153a51a4c3b77d8172ce37c6542c59"
SRC_URI[arm-libteecv1.sha256sum] = "bc65a13d234da8d4a9c0cfd6d0a8672e8fe1c1c884180f47121d41bd7dcefafe"

# Needed to resolve dependencies to libteec
RDEPENDS:${PN} += "optee-client"

inherit bin_package

INSANE_SKIP:${PN} = "already-stripped"
