# Copyright (C) 2025, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/alsa-lib:"
# override meta-freescale/recipes-multimedia/alsa/alsa-lib_%.bbappend
GLIBC_64BIT_TIME_FLAGS:arm:imx-nxp-bsp = " -D_TIME_BITS=64 -D_FILE_OFFSET_BITS=64"

SRC_URI:append = " file://0001-pcm-dmix-Fix-resume-with-multiple-instances.patch"
