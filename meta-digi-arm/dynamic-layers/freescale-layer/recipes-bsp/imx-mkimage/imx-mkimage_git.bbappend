# Copyright (C) 2022 Digi International

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append:ccimx8m = " \
    file://0001-imx8m-soc.mak-preserve-dtbs-after-build.patch \
"

# Use NXP's lf-6.1.22-2.0.0 release for ccimx93
SRC_URI:ccimx93 = "git://github.com/nxp-imx/imx-mkimage.git;protocol=https;branch=${SRCBRANCH}"
SRCBRANCH:ccimx93 = "lf-6.1.22_2.0.0"
SRCREV:ccimx93 = "5cfd218012e080fb907d9cc301fbb4ece9bc17a9"

# "fmacro-prefix-map" is not supported on old versions of GCC
DEBUG_PREFIX_MAP:remove:class-nativesdk = "-fmacro-prefix-map=${WORKDIR}=/usr/src/debug/${PN}/${EXTENDPE}${PV}-${PR}"

BBCLASSEXTEND = "native nativesdk"
