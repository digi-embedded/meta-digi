# Copyright (C) 2022-2024, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append:ccimx8m = " \
    file://0001-imx8m-soc.mak-preserve-dtbs-after-build.patch \
"

# "fmacro-prefix-map" is not supported on old versions of GCC
DEBUG_PREFIX_MAP:remove:class-nativesdk = "-fmacro-prefix-map=${WORKDIR}=/usr/src/debug/${PN}/${EXTENDPE}${PV}-${PR}"

BBCLASSEXTEND = "native nativesdk"
