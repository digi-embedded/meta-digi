# Copyright (C) 2022-2024, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append:ccimx8m = " \
    file://0001-imx8m-soc.mak-preserve-dtbs-after-build.patch \
"

# Use NXP's lf-6.1.55-2.2.0 release for ccimx9
SRC_URI:ccimx9 = "git://github.com/nxp-imx/imx-mkimage.git;protocol=https;branch=${SRCBRANCH}"
SRCBRANCH:ccimx9 = "lf-6.1.55_2.2.0"
SRCREV:ccimx9 = "c4365450fb115d87f245df2864fee1604d97c06a"

# "fmacro-prefix-map" is not supported on old versions of GCC
DEBUG_PREFIX_MAP:remove:class-nativesdk = "-fmacro-prefix-map=${WORKDIR}=/usr/src/debug/${PN}/${EXTENDPE}${PV}-${PR}"

BBCLASSEXTEND = "native nativesdk"
