# Copyright (C) 2022,2023 Digi International

SUMMARY = "Linux kernel for Digi boards"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

# CONFIG_KERNEL_LZO in defconfig
DEPENDS += "${@oe.utils.conditional('DEY_SOC_VENDOR', 'NXP', 'lzop-native', '', d)}"

inherit kernel
inherit ${@oe.utils.conditional('DEY_SOC_VENDOR', 'NXP', 'fsl-kernel-localversion', '', d)}

SRCBRANCH = "v5.15/nxp/dey-4.0/maint"
SRCBRANCH:stm32mpcommon = "v5.15/stm/dey-4.0/maint"
SRCREV = "0bc92de5498aa28a94de32259b6e0bffab9a2d68"
SRCREV:stm32mpcommon = "fba503649040ab79bcd113dc27a4e7e726ffed7c"

require ${@oe.utils.conditional('DEY_SOC_VENDOR', 'STM', 'recipes-kernel/linux/linux-stm32mp.inc', '', d)}
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

# Create base DTB suitable for overlays
OVERLAYS_DTC_FLAGS = "-@"
OVERLAYS_DTC_FLAGS:ccimx6ul = ""
KERNEL_DTC_FLAGS = "${OVERLAYS_DTC_FLAGS}"

KERNEL_EXTRA_ARGS:stm32mpcommon += "LOADADDR=${ST_KERNEL_LOADADDR}"

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8m|ccimx93|ccmp1)"
