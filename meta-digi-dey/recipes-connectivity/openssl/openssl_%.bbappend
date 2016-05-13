# Copyright (C) 2016 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI += " \
    file://0001-cryptodev-Fix-issue-with-signature-generation.patch \
    file://0002-cryptodev-allow-copying-EVP-contexts.patch \
"
