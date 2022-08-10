# Copyright (C) 2022 Digi International

SRCBRANCH = "MM_04.07.00_2205_L5.15.y"
SRCREV = "0f32bca96f7027c0c1145b27d1790541d34fb84c"

PACKAGECONFIG:append:mx8-nxp-bsp = " swpdm"
PACKAGECONFIG[swpdm] = "--enable-swpdm,--disable-swpdm,imx-sw-pdm"
