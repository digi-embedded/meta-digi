# Copyright (C) 2022 Digi International

SUMMARY = "Linux kernel for Digi boards"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

DEPENDS += "${@oe.utils.conditional('DEY_BUILD_PLATFORM', 'NXP', 'lzop-native', '', d)}"
DEPENDS += "${@oe.utils.conditional('DEY_BUILD_PLATFORM', 'NXP', 'bc-native', '', d)}"

inherit kernel
inherit ${@oe.utils.conditional('DEY_BUILD_PLATFORM', 'NXP', 'fsl-kernel-localversion', '', d)}

SRCBRANCH = "v5.10/nxp/master"
SRCBRANCH_stm32mpcommon = "v5.10/stm/master"

require ${@oe.utils.conditional('DEY_BUILD_PLATFORM', 'STM', 'recipes-kernel/linux/linux-stm32mp.inc', '', d)}
# Don't create custom folder for kernel artifacts
do_deploy[sstate-outputdirs] = "${DEPLOY_DIR_IMAGE}"

require recipes-kernel/linux/linux-dey-src.inc
require ${@bb.utils.contains('DISTRO_FEATURES', 'virtualization', 'linux-virtualization.inc', '', d)}
require ${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'recipes-kernel/linux/linux-trustfence.inc', '', d)}

# Use custom provided 'defconfig' if variable KERNEL_DEFCONFIG is cleared
SRC_URI +="${@oe.utils.conditional('KERNEL_DEFCONFIG', '', 'file://defconfig', '', d)}"

FILES_${KERNEL_PACKAGE_NAME}-image += "/boot/config-${KERNEL_VERSION}"

# Don't include kernels in standard images
RDEPENDS_${KERNEL_PACKAGE_NAME}-base = ""

do_install_append_stm32mpcommon() {
    if ${@bb.utils.contains('MACHINE_FEATURES','gpu','true','false',d)}; then
        # append evbug tool to blacklist
        echo "blacklist evbug" >> ${D}/${sysconfdir}/modprobe.d/blacklist.conf
    fi
}

# -------------------------------------------------------------
# Kernel Args
#
KERNEL_EXTRA_ARGS_stm32mpcommon += "LOADADDR=${ST_KERNEL_LOADADDR}"

COMPATIBLE_MACHINE = "(ccimx8mp|ccmp15-dvk)"
