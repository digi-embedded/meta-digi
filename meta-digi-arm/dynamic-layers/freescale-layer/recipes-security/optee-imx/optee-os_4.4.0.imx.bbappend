# Copyright (C) 2024, 2025, Digi International Inc.
FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI += "file://environment.d-optee-sdk.sh"

SRC_URI:append:dey = " \
    file://0001-plat-imx-add-support-for-ConnectCore-8M-Mini.patch \
    file://0002-core-imx-support-ccimx91-dvk.patch \
    file://0003-core-imx-support-ccimx93-dvk.patch \
    file://0004-core-ccimx93-enable-AES_HUK-trusted-application.patch \
    file://0005-core-imx-support-ccimx95-dvk.patch \
"

OPTEEMACHINE:ccimx8mm = "imx-ccimx8mmdvk"
OPTEEMACHINE:ccimx91 = "imx-ccimx91dvk"
OPTEEMACHINE:ccimx93 = "imx-ccimx93dvk"
OPTEEMACHINE:ccimx95 = "imx-ccimx95dvk"

do_compile:append:ccimx93 () {
    oe_runmake -C ${S} PLATFORM=imx-${PLATFORM_FLAVOR}_a0 O=${B}-A0
}
do_compile:ccimx93[cleandirs] += "${B}-A0"

do_deploy:append:ccimx93 () {
    cp ${B}-A0/core/tee-raw.bin ${DEPLOYDIR}/tee.${PLATFORM_FLAVOR}_a0.bin
}

do_install:append() {
    mkdir -p ${D}/environment-setup.d
    sed -e "s,#OPTEE_ARCH#,${OPTEE_ARCH},g" ${WORKDIR}/environment.d-optee-sdk.sh >${D}/environment-setup.d/optee-sdk.sh
}

FILES:${PN}-staticdev += "/environment-setup.d/"
INSANE_SKIP:${PN}-staticdev += "buildpaths"
