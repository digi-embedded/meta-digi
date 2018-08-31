# Copyright (C) 2013-2016 Freescale Semiconductor
# Copyright 2017-2018 NXP
# Released under the MIT license (see COPYING.MIT for the terms)

require imx-codec.inc

PACKAGECONFIG_remove_imxvpuamphion = "vpu"

LIC_FILES_CHKSUM = "file://COPYING;md5=ab61cab9599935bfe9f700405ef00f28"

SRC_URI[md5sum] = "27c4d8f70a2c9ee0c63034f97752c235"
SRC_URI[sha256sum] = "6f0117365e0b0235ba42fc8b1bbbc5e02e635da47aff66face5816721b581fbf"

COMPATIBLE_MACHINE = "(mx6|mx7|mx8)"
