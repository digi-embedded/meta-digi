# Copyright (C) 2016-2021 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

CRYPTOCHIP_COMMON_PATCHES = " \
    file://0003-Modify-openssl.cnf-to-automatically-load-the-cryptoc.patch \
"

SRC_URI += " \
    file://0001-cryptodev-Fix-issue-with-signature-generation.patch \
    file://0002-cryptodev-allow-copying-EVP-contexts.patch \
    ${@bb.utils.contains("MACHINE_FEATURES", "cryptochip", "${CRYPTOCHIP_COMMON_PATCHES}", "", d)} \
    file://version-script.patch \
    file://0001-DirectoryString-is-a-CHOICE-type-and-therefore-uses-.patch \
    file://0002-Correctly-compare-EdiPartyName-in-GENERAL_NAME_cmp.patch \
    file://0003-Check-that-multi-strings-CHOICE-types-don-t-use-impl.patch \
    file://0004-Complain-if-we-are-attempting-to-encode-with-an-inva.patch \
    file://0005-Fix-Null-pointer-deref-in-X509_issuer_and_serial_has.patch \
    file://0006-Don-t-overflow-the-output-length-in-EVP_CipherUpdate.patch \
"
