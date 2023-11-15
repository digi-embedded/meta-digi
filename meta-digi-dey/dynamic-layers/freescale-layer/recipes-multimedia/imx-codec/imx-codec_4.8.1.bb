# Copyright 2023 Digi International Inc.

#
# Reuse meta-freescale's imx-codec_4.7.2.bb
#
require recipes-multimedia/imx-codec/imx-codec_4.7.2.bb

LIC_FILES_CHKSUM = "file://COPYING;md5=db4762b09b6bda63da103963e6e081de"

SRC_URI[md5sum] = "a47f6407459ab4889e1bd9651b9dd40b"
SRC_URI[sha256sum] = "0d0668dadbd69c40c1d0e29cbf4082df008a7cb7ec7e5cfe7d8f228395bdaf58"

COMPATIBLE_MACHINE = "(ccimx93)"
