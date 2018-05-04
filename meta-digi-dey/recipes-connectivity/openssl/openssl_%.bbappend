# Copyright (C) 2016-2018 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI += " \
    file://0001-cryptodev-Fix-issue-with-signature-generation.patch \
    file://0002-cryptodev-allow-copying-EVP-contexts.patch \
"

CRYPTOCHIP_COMMON_PATCHES = " \
    file://0003-Modify-openssl.cnf-to-automatically-load-the-cryptoc.patch \
"

SRC_URI_append_ccimx6ul = " ${CRYPTOCHIP_COMMON_PATCHES}"
SRC_URI_append_ccimx6qpsbc = " ${CRYPTOCHIP_COMMON_PATCHES}"
