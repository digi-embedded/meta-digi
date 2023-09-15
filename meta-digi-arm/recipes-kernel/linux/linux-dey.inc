# Copyright (C) 2018-2023 Digi International
SUMMARY = "Linux kernel for Digi boards"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

inherit kernel
inherit ${@oe.utils.conditional('DEY_SOC_VENDOR', 'NXP', 'fsl-kernel-localversion', '', d)}
require ${@oe.utils.conditional('DEY_SOC_VENDOR', 'STM', 'recipes-kernel/linux/linux-stm32mp.inc', '', d)}
require ${@bb.utils.contains('DISTRO_FEATURES', 'virtualization', 'linux-virtualization.inc', '', d)}
require ${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'recipes-kernel/linux/linux-trustfence.inc', '', d)}

# CONFIG_KERNEL_LZO in defconfig
DEPENDS += "${@oe.utils.conditional('DEY_SOC_VENDOR', 'NXP', 'lzop-native', '', d)}"

# Select internal or Github Linux repo
LINUX_URI_STASH = "${DIGI_MTK_GIT}/linux/linux.git;protocol=ssh"
LINUX_URI_GITHUB = "${DIGI_GITHUB_GIT}/linux.git;protocol=https"
LINUX_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1', '${LINUX_URI_STASH}', '${LINUX_URI_GITHUB}', d)}"
SRC_URI = " \
    ${LINUX_GIT_URI};branch=${SRCBRANCH} \
    ${@oe.utils.conditional('KERNEL_DEFCONFIG', '', 'file://defconfig', '', d)} \
"
S = "${WORKDIR}/git"

LOCALVERSION = "-dey"

# Create base DTB suitable for overlays
OVERLAYS_DTC_FLAGS = "-@"
OVERLAYS_DTC_FLAGS:ccimx6ul = ""
OVERLAYS_DTC_FLAGS:ccimx6 = ""
KERNEL_DTC_FLAGS = "${OVERLAYS_DTC_FLAGS}"

KERNEL_EXTRA_ARGS:stm32mpcommon += "LOADADDR=${ST_KERNEL_LOADADDR}"

# A user can provide his own kernel 'defconfig' file by:
# - setting the variable KERNEL_DEFCONFIG to a custom kernel configuration file
#   inside the kernel repository.
# - setting the variable KERNEL_DEFCONFIG to a kernel configuration file using
#   the full path to the file.
# - clearing the variable KERNEL_DEFCONFIG and providing a kernel configuration
#   file in the layer (in this case the file must be named 'defconfig').
#
# Otherwise the default platform's kernel configuration file will be taken from
# the Linux source code tree.
do_copy_defconfig[vardeps] += "KERNEL_DEFCONFIG"
do_copy_defconfig[dirs] = "${S}"
do_copy_defconfig () {
	if [ -n "${KERNEL_DEFCONFIG}" ]; then
		cp -f ${KERNEL_DEFCONFIG} ${WORKDIR}/defconfig
	fi
}
addtask copy_defconfig after do_patch before do_kernel_localversion

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

# Don't create custom folder for kernel artifacts
do_deploy[sstate-outputdirs] = "${DEPLOY_DIR_IMAGE}"
