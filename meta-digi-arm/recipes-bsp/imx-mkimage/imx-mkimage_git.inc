# Copyright 2017-2018 NXP

IMX_MKIMAGE_SRC ?= "git://github.com/nxp-imx/imx-mkimage.git;protocol=https"
SRCBRANCH = "imx_4.9.88_imx8qxp_beta2"
SRC_URI = "${IMX_MKIMAGE_SRC};branch=${SRCBRANCH}"
SRCREV = "5c18f544c460747daed2844714c49685ed6ce897"

DEPENDS = "zlib-native openssl-native"
