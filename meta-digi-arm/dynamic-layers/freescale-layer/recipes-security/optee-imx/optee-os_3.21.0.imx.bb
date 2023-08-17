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
SRCBRANCH = "lf-6.1.22_2.0.0"
SRCREV = "1962aec9581760803b1485d455cd62cb11c14870"

PLATFORM_FLAVOR:ccimx93 = "ccimx93dvk"

COMPATIBLE_MACHINE = "(ccimx93)"
