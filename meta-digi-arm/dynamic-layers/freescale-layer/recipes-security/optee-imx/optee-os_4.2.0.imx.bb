# Copyright (C) 2024, Digi International Inc.

#
# Reuse meta-freescale's optee-os_3.19.0.imx.bb
#
require recipes-security/optee-imx/optee-os_3.19.0.imx.bb

SRC_URI = " \
    git://github.com/nxp-imx/imx-optee-os.git;protocol=https;branch=${SRCBRANCH} \
    file://0007-allow-setting-sysroot-for-clang.patch \
    file://0001-core-imx-support-ccimx91-dvk.patch \
    file://environment.d-optee-sdk.sh \
"
SRCBRANCH = "lf-6.6.23_2.0.0"
# Tag: lf-6.6.23-2.0.0
SRCREV = "c6be5b572452a2808d1a34588fd10e71715e23cf"

PLATFORM_FLAVOR:ccimx91 = "ccimx91dvk"

do_install:append:ccimx91 () {
	mkdir -p ${D}/environment-setup.d
	sed -e "s,#OPTEE_ARCH#,${OPTEE_ARCH},g" ${WORKDIR}/environment.d-optee-sdk.sh > ${D}/environment-setup.d/optee-sdk.sh
}

FILES:${PN}-staticdev += "/environment-setup.d/"

COMPATIBLE_MACHINE = "(ccimx91)"
