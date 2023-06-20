# Copyright (C) 2023 Digi International

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

# Use NXP's lf-6.1.1_1.0.0 release for ccimx93
SRC_URI:append:ccimx93 = " file://0001-core-imx-support-ccimx93-dvk.patch"
SRCBRANCH:ccimx93 = "lf-6.1.1_1.0.0"
SRCREV:ccimx93 = "ad4e8389bb2c38efe39853925eec571ac778c575"

PLATFORM_FLAVOR:ccimx93 = "ccimx93dvk"
