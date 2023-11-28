# Copyright 2023 Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

LIC_FILES_CHKSUM = "\
    file://LICENSE.txt;md5=e3fc50a88d0a364313df4b21ef20c29e \
    file://LICENSE-GPL-2.0.txt;md5=b234ee4d69f5fce4486a80fdaf4a4263 \
    file://LICENSE-BSD-3.txt;md5=0858ec9c7a80c4a2cf16e4f825a2cc91 \
"

SRCBRANCH = "lf-6.1.36_2.1.0"
SRCREV = "5fff874731d02bb232159108ccfa6833e92b6942"
SRC_URI:append:ccimx93 = " file://ethosu_firmware"

do_install:append:ccimx93 () {
    install -m 0644 ${WORKDIR}/ethosu_firmware ${D}${nonarch_base_libdir}/firmware
}
