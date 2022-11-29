# Copyright (C) 2022 Digi International

SUMMARY = "Linux kernel for Digi boards"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

# CONFIG_KERNEL_LZO in defconfig
DEPENDS += "${@oe.utils.conditional('DEY_BUILD_PLATFORM', 'NXP', 'lzop-native', '', d)}"

inherit kernel
inherit ${@oe.utils.conditional('DEY_BUILD_PLATFORM', 'NXP', 'fsl-kernel-localversion', '', d)}

SRCBRANCH = "v5.15/nxp/dey-4.0/maint"
SRCBRANCH:stm32mpcommon = "v5.15/stm/dey-4.0/maint"
SRCREV = "55e3f4eb30296a8cc0df9392ffd630a10348e854"
SRCREV:stm32mpcommon = "5d06c7f6d3a5863b3be3fdb7201167f8af593059"

require ${@oe.utils.conditional('DEY_BUILD_PLATFORM', 'STM', 'recipes-kernel/linux/linux-stm32mp.inc', '', d)}
# Don't create custom folder for kernel artifacts
do_deploy[sstate-outputdirs] = "${DEPLOY_DIR_IMAGE}"

require recipes-kernel/linux/linux-dey-src.inc
require ${@bb.utils.contains('DISTRO_FEATURES', 'virtualization', 'linux-virtualization.inc', '', d)}
require ${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'recipes-kernel/linux/linux-trustfence.inc', '', d)}

# Use custom provided 'defconfig' if variable KERNEL_DEFCONFIG is cleared
SRC_URI +="${@oe.utils.conditional('KERNEL_DEFCONFIG', '', 'file://defconfig', '', d)}"

# This is needed because kernel_localversion (in fsl-kernel-localversion.bbclass)
# creates a basic ${B}/.config file and because that file exists, kernel_do_configure
# (in kernel.bbclass) does not apply our defconfig.
do_configure:prepend:imx-nxp-bsp() {
	if [ -f "${WORKDIR}/defconfig" ] && [ -f "${B}/.config" ]; then
		cat "${WORKDIR}/defconfig" >> "${B}/.config"
	fi
}

# Apply configuration fragments
do_configure:append() {
	# Only accept fragments ending in .cfg. If the fragments contain
	# something other than kernel configs, it will be filtered out
	# automatically.
	if [ -n "${@' '.join(find_cfgs(d))}" ]; then
		${S}/scripts/kconfig/merge_config.sh -m -O ${B} ${B}/.config ${@" ".join(find_cfgs(d))}
	fi
}

KERNEL_EXTRA_ARGS:stm32mpcommon += "LOADADDR=${ST_KERNEL_LOADADDR}"

COMPATIBLE_MACHINE = "(ccimx6ul|ccmp1)"
