# Copyright (C) 2016-2019 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

CRYPTOCHIP_COMMON_PATCHES = " \
    file://0001-Modify-openssl.cnf-to-automatically-load-the-pkcs11-.patch \
"

SRC_URI_remove = " \
    file://debian1.0.2/version-script.patch \
    file://debian1.0.2/soname.patch \
"

SRC_URI += " \
    ${@bb.utils.contains("MACHINE_FEATURES", "cryptochip", "${CRYPTOCHIP_COMMON_PATCHES}", "", d)} \
"

# Add the openssl binary whenever the base package is pulled in via a dependency
RRECOMMENDS_libcrypto += "openssl-bin"
