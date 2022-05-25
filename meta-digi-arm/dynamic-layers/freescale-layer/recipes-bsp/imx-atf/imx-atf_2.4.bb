# Copyright (C) 2017-2021 NXP

DESCRIPTION = "i.MX ARM Trusted Firmware"
SECTION = "BSP"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/BSD-3-Clause;md5=550794465ba0ec5312d6919e203a55f9"

PV .= "+git${SRCPV}"

SRCBRANCH = "lf_v2.4"
ATF_SRC ?= "git://source.codeaurora.org/external/imx/imx-atf.git;protocol=https"
SRC_URI = "${ATF_SRC};branch=${SRCBRANCH} \
"
SRCREV = "5782363f92a2fdf926784449270433cf3ddf44bd"

SRC_URI:append:ccimx8mn = " file://0001-imx8mn-Define-UART1-as-console-for-boot-stage.patch \
                            file://0002-imx8mn-Disable-M7-debug-console.patch"
SRC_URI:append:ccimx8mm = " file://0001-imx8mm-Define-UART1-as-console-for-boot-stage.patch \
                            file://0002-imx8mm-Disable-M4-debug-console.patch"

S = "${WORKDIR}/git"

inherit deploy

BOOT_TOOLS = "imx-boot-tools"

PLATFORM        ?= "INVALID"
PLATFORM:mx8qm-nxp-bsp   = "imx8qm"
PLATFORM:mx8x-nxp-bsp    = "imx8qx"
PLATFORM:mx8mq-nxp-bsp   = "imx8mq"
PLATFORM:mx8mm-nxp-bsp  = "imx8mm"
PLATFORM:mx8mn-nxp-bsp  = "imx8mn"
PLATFORM:mx8mnul-nxp-bsp = "imx8mn"
PLATFORM:mx8mp-nxp-bsp  = "imx8mp"
PLATFORM:mx8mpul-nxp-bsp = "imx8mp"
PLATFORM:mx8dx-nxp-bsp   = "imx8dx"
PLATFORM:mx8dxl-nxp-bsp  = "imx8dxl"
PLATFORM:mx8ulp-nxp-bsp  = "imx8ulp"

# Clear LDFLAGS to avoid the option -Wl recognize issue
# Clear CFLAGS to avoid coherent_arm out of OCRAM size limitation (64KB) - i.MX 8MQ only
CLEAR_FLAGS ?= "LDFLAGS"
CLEAR_FLAGS:mx8mq-nxp-bsp = "LDFLAGS CFLAGS"

EXTRA_OEMAKE += " \
    CROSS_COMPILE="${TARGET_PREFIX}" \
    PLAT=${PLATFORM} \
"

BUILD_OPTEE = "${@bb.utils.contains('MACHINE_FEATURES', 'optee', 'true', 'false', d)}"

do_compile() {
    unset ${CLEAR_FLAGS}

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
COMPATIBLE_MACHINE = "(mx8-nxp-bsp)"
