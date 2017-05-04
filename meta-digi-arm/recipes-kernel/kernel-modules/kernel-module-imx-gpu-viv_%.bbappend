# Copyright (C) 2015-2017 Digi International

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI += " \
	file://0001-mxc-gpu-use-recommended-values-for-minimum-GPU-frequ.patch \
	file://0002-Use-busfreq-imx6.h-up-to-3.15-kernel.patch \
	file://0003-gpu-Get-GPU-reserved-memory-from-DT.patch \
"
