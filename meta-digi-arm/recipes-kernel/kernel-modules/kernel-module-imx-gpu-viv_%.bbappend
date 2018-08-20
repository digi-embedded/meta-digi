# Copyright (C) 2017-2018 Digi International

VER_DIR = "${@d.getVar('PV', True).split('+git')[0]}"

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}-${VER_DIR}:"

SRC_URI += " \
    file://0001-mxc-gpu-use-recommended-values-for-minimum-GPU-frequ.patch \
"
