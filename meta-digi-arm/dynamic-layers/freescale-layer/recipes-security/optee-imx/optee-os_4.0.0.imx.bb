# Copyright 2023 Digi International Inc.

#
# Reuse meta-freescale's optee-os_3.19.0.imx.bb
#
require recipes-security/optee-imx/optee-os_3.19.0.imx.bb

SRC_URI = " \
    git://github.com/nxp-imx/imx-optee-os.git;protocol=https;branch=${SRCBRANCH} \
    file://0007-allow-setting-sysroot-for-clang.patch \
    file://0001-core-imx-support-ccimx93-dvk.patch \
"
SRCBRANCH = "lf-6.1.55_2.2.0"
# Tag: lf-6.1.55-2.2.0
SRCREV = "a303fc80f7c4bd713315687a1fa1d6ed136e78ee"

PLATFORM_FLAVOR:ccimx93 = "ccimx93dvk"

do_compile:append:ccimx93 () {
    oe_runmake PLATFORM=imx-${PLATFORM_FLAVOR}_a0 O=${B}-A0 all
}
do_compile[cleandirs] += "${B}-A0"

do_deploy:append:ccimx93 () {
    cp ${B}-A0/core/tee-raw.bin ${DEPLOYDIR}/tee.${PLATFORM_FLAVOR}_a0.bin
}

COMPATIBLE_MACHINE = "(ccimx93)"
