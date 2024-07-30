# Copyright 2024 Digi International Inc.

#
# Reuse meta-freescale's imx-codec_4.7.2.bb
#
require recipes-multimedia/imx-codec/imx-codec_4.7.2.bb

LIC_FILES_CHKSUM = "file://COPYING;md5=2827219e81f28aba7c6a569f7c437fa7"

SRC_URI[md5sum] = "1977bab8d89972f08d9eee0122a64603"
SRC_URI[sha256sum] = "b0744a91c265202a79a019c72f17cae01fd5b63a3ba451592b6c8349d95719e0"

COMPATIBLE_MACHINE = "(ccimx9)"
