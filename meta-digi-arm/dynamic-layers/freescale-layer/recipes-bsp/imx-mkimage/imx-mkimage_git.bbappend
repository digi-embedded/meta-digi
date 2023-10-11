# Copyright (C) 2022 Digi International

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append:ccimx8m = " \
    file://0001-imx8m-soc.mak-preserve-dtbs-after-build.patch \
"

# Use NXP's lf-6.1.36-2.1.0 release for ccimx93
SRC_URI:ccimx93 = "git://github.com/nxp-imx/imx-mkimage.git;protocol=https;branch=${SRCBRANCH}"
SRCBRANCH:ccimx93 = "lf-6.1.36_2.1.0"
SRCREV:ccimx93 = "5a0faefc223e51e088433663b6e7d6fbce89bf59"

# "fmacro-prefix-map" is not supported on old versions of GCC
DEBUG_PREFIX_MAP:remove:class-nativesdk = "-fmacro-prefix-map=${WORKDIR}=/usr/src/debug/${PN}/${EXTENDPE}${PV}-${PR}"

BBCLASSEXTEND = "native nativesdk"
