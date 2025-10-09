# Copyright (C) 2021-2025, Digi International Inc.

# override meta-freescale/recipes-multimedia/gstreamer/gstreamer1.0-plugins-good_1.24.7.imx.bb
GLIBC_64BIT_TIME_FLAGS:arm:imx-nxp-bsp = " -D_TIME_BITS=64 -D_FILE_OFFSET_BITS=64"
INSANE_SKIP:remove:imx-nxp-bsp = "32bit-time"

PACKAGECONFIG:append:ccimx6ul = " vpx"
