# Copyright 2017-2018 NXP

DESCRIPTION = "Generate Boot Loader for i.MX8 device"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"
SECTION = "BSP"

require imx-mkimage_git.inc

inherit deploy

# Add CFLAGS with native INCDIR & LIBDIR for imx-mkimage build
CFLAGS = "-O2 -Wall -std=c99 -static -I ${STAGING_INCDIR_NATIVE} -L ${STAGING_LIBDIR_NATIVE}"

BOOT_TOOLS = "imx-boot-tools"
BOOT_NAME = "imx-boot"
PROVIDES = "${BOOT_NAME}"

IMX_FIRMWARE = "firmware-imx digi-sc-firmware"
DEPENDS += " \
    u-boot \
    ${IMX_FIRMWARE} \
    imx-atf \
    ${@bb.utils.contains('COMBINED_FEATURES', 'optee', 'optee-os-imx', '', d)} \
"
DEPENDS_append_ccimx8x = " coreutils-native"

# For i.MX 8, this package aggregates the imx-m4-demos
# output. Note that this aggregation replaces the aggregation
# that would otherwise be done in the image build as controlled
# by IMAGE_BOOTFILES_DEPENDS and IMAGE_BOOTFILES in image_types_fsl.bbclass
IMX_M4_DEMOS = "imx-m4-demos"

# This package aggregates output deployed by other packages,
# so set the appropriate dependencies
do_compile[depends] += " \
    virtual/bootloader:do_deploy \
    ${@' '.join('%s:do_deploy' % r for r in '${IMX_FIRMWARE}'.split() )} \
    imx-atf:do_deploy \
    ${@' '.join('%s:do_deploy' % r for r in '${IMX_M4_DEMOS}'.split() )} \
    ${@bb.utils.contains('COMBINED_FEATURES', 'optee', 'optee-os-imx:do_deploy', '', d)} \
"

# This package aggregates dependencies with other packages,
# so also define the license dependencies.
do_populate_lic[depends] += " \
    virtual/bootloader:do_populate_lic \
    ${@' '.join('%s:do_populate_lic' % r for r in '${IMX_FIRMWARE}'.split() )} \
    imx-atf:do_populate_lic \
    ${@' '.join('%s:do_populate_lic' % r for r in '${IMX_M4_DEMOS}'.split() )} \
    ${@bb.utils.contains('COMBINED_FEATURES', 'optee', 'optee-os-imx:do_populate_lic', '', d)} \
"

SC_FIRMWARE_NAME ?= "scfw_tcm.bin"

ATF_MACHINE_NAME = "bl31-imx8qxp.bin"
ATF_MACHINE_NAME_append = "${@bb.utils.contains('COMBINED_FEATURES', 'optee', '-optee', '', d)}"

UBOOT_NAME = "u-boot-${MACHINE}.bin"
BOOT_CONFIG_MACHINE = "${BOOT_NAME}"

TOOLS_NAME ?= "mkimage_imx8"

SOC_TARGET = "iMX8QX"

SOC_DIR ?= "${SOC_TARGET}"

DEPLOY_OPTEE = "${@bb.utils.contains('COMBINED_FEATURES', 'optee', 'true', 'false', d)}"

IMXBOOT_TARGETS = "${@bb.utils.contains('UBOOT_CONFIG', 'fspi', 'flash_flexspi', \
                       bb.utils.contains('UBOOT_CONFIG', 'nand', 'flash_nand', \
                                                                 'flash flash_all', d), d)}"

S = "${WORKDIR}/git"

do_compile () {
    echo 8QX boot binary build
    cp ${DEPLOY_DIR_IMAGE}/imx8qx_m4_TCM_srtm_demo.bin ${S}/${SOC_DIR}/m40_tcm.bin
    cp ${DEPLOY_DIR_IMAGE}/imx8qx_m4_TCM_srtm_demo.bin ${S}/${SOC_DIR}/CM4.bin
    cp ${DEPLOY_DIR_IMAGE}/mx8qx-ahab-container.img ${S}/${SOC_DIR}/
    cp ${DEPLOY_DIR_IMAGE}/${BOOT_TOOLS}/${ATF_MACHINE_NAME} ${S}/${SOC_DIR}/bl31.bin
    for type in ${UBOOT_CONFIG}; do
        RAM_SIZE="$(echo ${type} | sed -e 's,.*\([0-9]\+GB\),\1,g')"
        cp ${DEPLOY_DIR_IMAGE}/${BOOT_TOOLS}/${UBOOT_NAME}-${type}         ${S}/${SOC_DIR}/u-boot.bin-${type}
        cp ${DEPLOY_DIR_IMAGE}/${BOOT_TOOLS}/${SC_FIRMWARE_NAME}-${RAM_SIZE} ${S}/${SOC_DIR}/scfw_tcm.bin-${RAM_SIZE}
    done

    # Copy TEE binary to SoC target folder to mkimage
    if ${DEPLOY_OPTEE}; then
        cp ${DEPLOY_DIR_IMAGE}/tee.bin             ${S}/${SOC_DIR}/
    fi

    # mkimage for i.MX8
    for type in ${UBOOT_CONFIG}; do
        cd ${S}/${SOC_DIR}
        ln -sf u-boot.bin-${type} u-boot.bin
        RAM_SIZE="$(echo ${type} | sed -e 's,.*\([0-9]\+GB\),\1,g')"
        ln -sf scfw_tcm.bin-${RAM_SIZE} scfw_tcm.bin
        cd -
        for target in ${IMXBOOT_TARGETS}; do
            echo "building ${SOC_TARGET} - ${type} - ${target}"
            make SOC=${SOC_TARGET} ${target}
            if [ -e "${S}/${SOC_DIR}/flash.bin" ]; then
                cp ${S}/${SOC_DIR}/flash.bin ${S}/${BOOT_CONFIG_MACHINE}-${type}.bin-${target}
            fi
        done
        rm ${S}/${SOC_DIR}/scfw_tcm.bin
        rm ${S}/${SOC_DIR}/u-boot.bin
        # Remove u-boot-atf.bin so it gets generated with the next iteration's U-Boot
        rm ${S}/${SOC_DIR}/u-boot-atf.bin
    done
}

SYSROOT_DIRS += "/boot"

do_install () {
    install -d ${D}/boot
    for type in ${UBOOT_CONFIG}; do
        for target in ${IMXBOOT_TARGETS}; do
            install -m 0644 ${S}/${BOOT_CONFIG_MACHINE}-${type}.bin-${target} ${D}/boot/
        done
    done
}

DEPLOYDIR_IMXBOOT = "${BOOT_TOOLS}"
do_deploy () {
    install -d ${DEPLOYDIR}/${DEPLOYDIR_IMXBOOT}

    # copy the tool mkimage to deploy path and sc fw, dcd and uboot
    install -m 0644 ${S}/${SOC_DIR}/mx8qx-ahab-container.img ${DEPLOYDIR}/${DEPLOYDIR_IMXBOOT}
    install -m 0644 ${S}/${SOC_DIR}/m40_tcm.bin ${DEPLOYDIR}/${DEPLOYDIR_IMXBOOT}
    install -m 0644 ${S}/${SOC_DIR}/CM4.bin ${DEPLOYDIR}/${DEPLOYDIR_IMXBOOT}

    install -m 0755 ${S}/${TOOLS_NAME} ${DEPLOYDIR}/${BOOT_TOOLS}

    # copy tee.bin to deploy path
    if "${DEPLOY_OPTEE}"; then
        install -m 0644 ${DEPLOY_DIR_IMAGE}/tee.bin ${DEPLOYDIR}/${DEPLOYDIR_IMXBOOT}
    fi

    # copy makefile (soc.mak) for reference
    install -m 0644 ${S}/${SOC_DIR}/soc.mak     ${DEPLOYDIR}/${DEPLOYDIR_IMXBOOT}

    # copy the generated boot image to deploy path
    for type in ${UBOOT_CONFIG}; do
        IMAGE_IMXBOOT_TARGET=""
        for target in ${IMXBOOT_TARGETS}; do
            # Use first "target" as IMAGE_IMXBOOT_TARGET
            if [ "$IMAGE_IMXBOOT_TARGET" = "" ]; then
                IMAGE_IMXBOOT_TARGET="$target"
                echo "Set boot target as $IMAGE_IMXBOOT_TARGET"
            fi
            install -m 0644 ${S}/${BOOT_CONFIG_MACHINE}-${type}.bin-${target} ${DEPLOYDIR}
        done
        cd ${DEPLOYDIR}
        ln -sf ${BOOT_CONFIG_MACHINE}-${type}.bin-${IMAGE_IMXBOOT_TARGET} ${BOOT_CONFIG_MACHINE}-${type}.bin
        ln -sf ${BOOT_CONFIG_MACHINE}-${type}.bin-${IMAGE_IMXBOOT_TARGET} ${BOOT_CONFIG_MACHINE}-${MACHINE}.bin
        cd -
    done
}

addtask deploy before do_build after do_compile

FILES_${PN} = "/boot"

COMPATIBLE_MACHINE = "(mx8qxp)"
PACKAGE_ARCH = "${MACHINE_ARCH}"
