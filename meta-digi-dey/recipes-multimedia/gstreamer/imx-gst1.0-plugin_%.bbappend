# Copyright (C) 2016-2018 Digi International

FILESEXTRAPATHS_prepend := "${THISDIR}/${BP}:"

SRC_URI += "file://0001-gstimxv4l2-map-dev-video1-to-dev-fb0.patch"

SRC_URI_append_ccimx6 = " file://0002-imx_2d_device_g2d-define-a-G2D_AMPHION_INTERLACED-ma.patch"
