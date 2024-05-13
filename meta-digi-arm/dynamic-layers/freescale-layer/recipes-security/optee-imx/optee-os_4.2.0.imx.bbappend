# Copyright (C) 2024, Digi International Inc.
FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:append:ccimx91 = " \
    file://0001-core-imx-support-ccimx91-dvk.patch \
    file://environment.d-optee-sdk.sh \
"

SRC_URI:append:ccimx93 = " \
    file://0001-core-imx-support-ccimx93-dvk.patch \
    file://0002-core-ccimx93-enable-AES_HUK-trusted-application.patch \
"

PLATFORM_FLAVOR:ccimx91 = "ccimx91dvk"
PLATFORM_FLAVOR:ccimx93 = "ccimx93dvk"

do_install:append:ccimx91 () {
	mkdir -p ${D}/environment-setup.d
	sed -e "s,#OPTEE_ARCH#,${OPTEE_ARCH},g" ${WORKDIR}/environment.d-optee-sdk.sh > ${D}/environment-setup.d/optee-sdk.sh
}

FILES:${PN}-staticdev:ccimx91 += "/environment-setup.d/"

do_compile:append:ccimx93 () {
    oe_runmake PLATFORM=imx-${PLATFORM_FLAVOR}_a0 O=${B}-A0 all
}
do_compile:ccimx93[cleandirs] += "${B}-A0"

do_deploy:append:ccimx93 () {
    cp ${B}-A0/core/tee-raw.bin ${DEPLOYDIR}/tee.${PLATFORM_FLAVOR}_a0.bin
}
