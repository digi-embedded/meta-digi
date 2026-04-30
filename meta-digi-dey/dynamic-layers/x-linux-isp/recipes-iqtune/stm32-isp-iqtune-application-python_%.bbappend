# Copyright (C) 2026, Digi International Inc.

#
# We need to get the rest of SRC_URI artifacts from meta-st-x-linux-isp, so
# "abuse" COREBASE to get the path to "meta-st-x-linux-isp"
#
FILESEXTRAPATHS:prepend := "${THISDIR}/files:${COREBASE}/../meta-st-x-linux-isp/recipes-iqtune/files:"

SRC_URI += " \
    file://patches/0001-stm32_isp_iqtune-add-support-to-ConnectCore-MP2.patch \
"

DEPENDS:remove = "linux-stm32mp"
DEPENDS:append = " virtual/kernel "
