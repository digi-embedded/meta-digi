# Copyright (C) 2017-2021 NXP

DESCRIPTION = "i.MX ARM Trusted Firmware"
SECTION = "BSP"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/BSD-3-Clause;md5=550794465ba0ec5312d6919e203a55f9"

PV .= "+git${SRCPV}"

SRCBRANCH = "imx_5.4.70_2.3.0"
ATF_SRC ?= "git://github.com/nxp-imx/imx-atf.git;protocol=https"
SRC_URI = "${ATF_SRC};branch=${SRCBRANCH} \
"
SRCREV = "f1d7187f261ebf4b8a2a70d638d4bfc0a9b26c29"

SRC_URI_append_ccimx8mn = " file://0001-imx8mn-Define-UART1-as-console-for-boot-stage.patch \
                            file://0002-imx8mn-Disable-M7-debug-console.patch"
SRC_URI_append_ccimx8mm = " file://0001-imx8mm-Define-UART1-as-console-for-boot-stage.patch \
                            file://0002-imx8mm-Disable-M4-debug-console.patch"
SRC_URI_append_ccimx8m = " file://0003-TEE-639-plat-imx8m-Do-not-release-JR0-to-NS-if-HAB-i.patch"

S = "${WORKDIR}/git"

inherit deploy

BOOT_TOOLS = "imx-boot-tools"

PLATFORM        ?= "INVALID"
PLATFORM_mx8dxl = "imx8dxl"
PLATFORM_mx8dx  = "imx8dx"
PLATFORM_mx8dxlevk-phantom = "imx8qx"
PLATFORM_mx8mm  = "imx8mm"
PLATFORM_mx8mn  = "imx8mn"
PLATFORM_mx8mnlite  = "imx8mn"
PLATFORM_mx8mp  = "imx8mp"
PLATFORM_mx8mq  = "imx8mq"
PLATFORM_mx8qm  = "imx8qm"
PLATFORM_mx8x   = "imx8qx"


EXTRA_OEMAKE += " \
    CROSS_COMPILE="${TARGET_PREFIX}" \
    PLAT=${PLATFORM} \
"

BUILD_OPTEE = "${@bb.utils.contains('MACHINE_FEATURES', 'optee', 'true', 'false', d)}"

do_compile() {
    # Clear LDFLAGS to avoid the option -Wl recognize issue
    unset LDFLAGS
    oe_runmake bl31
    if ${BUILD_OPTEE}; then
       oe_runmake clean BUILD_BASE=build-optee
       oe_runmake BUILD_BASE=build-optee SPD=opteed bl31
    fi
}

do_install[noexec] = "1"

do_deploy() {
    install -Dm 0644 ${S}/build/${PLATFORM}/release/bl31.bin ${DEPLOYDIR}/${BOOT_TOOLS}/bl31-${PLATFORM}.bin
    if ${BUILD_OPTEE}; then
       install -m 0644 ${S}/build-optee/${PLATFORM}/release/bl31.bin ${DEPLOYDIR}/${BOOT_TOOLS}/bl31-${PLATFORM}.bin-optee
    fi
}
addtask deploy after do_compile

PACKAGE_ARCH = "${MACHINE_SOCARCH}"
COMPATIBLE_MACHINE = "(mx8)"
