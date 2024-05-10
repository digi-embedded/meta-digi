# Copyright 2024 Digi International Inc.

#
# Reuse meta-imx/meta-ml ethos-u-firmware_22.08.bb
#
require recipes-libraries/ethos-u-driver-stack/ethos-u-firmware_22.08.bb

LIC_FILES_CHKSUM = "\
    file://LICENSE.txt;md5=e3fc50a88d0a364313df4b21ef20c29e \
    file://LICENSE-GPL-2.0.txt;md5=b234ee4d69f5fce4486a80fdaf4a4263 \
    file://LICENSE-BSD-3.txt;md5=0858ec9c7a80c4a2cf16e4f825a2cc91 \
"

SRCBRANCH = "lf-6.1.55_2.2.0"
SRCREV = "7639c9a8ded082c642ff86e55ca053950e6b2486"
SRC_URI:append:ccimx93 = " file://ethosu_firmware"

do_install:ccimx93 () {
    install -d ${D}${nonarch_base_libdir}/firmware
    install -m 0644 ${WORKDIR}/ethosu_firmware ${D}${nonarch_base_libdir}/firmware/ethosu_firmware
}
