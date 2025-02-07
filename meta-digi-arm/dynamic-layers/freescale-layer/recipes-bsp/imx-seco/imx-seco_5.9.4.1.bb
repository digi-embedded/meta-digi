# Copyright (C) 2025, Digi International Inc.

#
# Reuse meta-freescale's imx-seco_5.9.4.bb
#
require recipes-bsp/imx-seco/imx-seco_5.9.4.bb

LIC_FILES_CHKSUM = "file://COPYING;md5=ca53281cc0caa7e320d4945a896fb837"

SRC_URI[md5sum] = "2a8fcdd322713bc127398ee66bf9b50a"
SRC_URI[sha256sum] = "bd8dc01966076836aabff53f2463295294166595006e1db430db21b6ffa6b667"
IMX_SRCREV_ABBREV = "0333596"

inherit fsl-eula2-unpack2 fsl-eula-recent
