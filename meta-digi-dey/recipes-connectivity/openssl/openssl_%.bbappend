# Copyright (C) 2016-2018 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

CRYPTOCHIP_COMMON_PATCHES = " \
    file://0003-Modify-openssl.cnf-to-automatically-load-the-cryptoc.patch \
"

SRC_URI += " \
    file://0001-cryptodev-Fix-issue-with-signature-generation.patch \
    file://0002-cryptodev-allow-copying-EVP-contexts.patch \
    ${@bb.utils.contains("MACHINE_FEATURES", "cryptochip", "${CRYPTOCHIP_COMMON_PATCHES}", "", d)} \
"
