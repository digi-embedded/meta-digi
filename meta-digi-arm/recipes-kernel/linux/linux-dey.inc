# Copyright (C) 2018-2024, Digi International Inc.
SUMMARY = "Linux kernel for Digi boards"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

inherit kernel
require ${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'recipes-kernel/linux/linux-trustfence.inc', '', d)}

# CONFIG_KERNEL_LZO in defconfig
DEPENDS += "${@oe.utils.conditional('DEY_SOC_VENDOR', 'NXP', 'lzop-native', '', d)}"

SRC_URI = " \
    ${LINUX_GIT_URI};branch=${SRCBRANCH} \
    ${@oe.utils.conditional('KERNEL_DEFCONFIG', '', 'file://defconfig', '', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'virtualization', 'file://docker_conf.cfg', '', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'tsn', 'file://tsn_conf.cfg', '', d)} \
    ${@oe.utils.vartrue('TRUSTFENCE_FILE_BASED_ENCRYPT', 'file://fscrypt.cfg', '', d)} \
"
S = "${WORKDIR}/git"

KERNEL_LOCALVERSION = "-dey"

# Create base DTB suitable for overlays
OVERLAYS_DTC_FLAGS = "-@"
OVERLAYS_DTC_FLAGS:ccimx6ul = ""
OVERLAYS_DTC_FLAGS:ccimx6 = ""
KERNEL_DTC_FLAGS = "${OVERLAYS_DTC_FLAGS}"

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
addtask copy_defconfig before do_configure after do_patch

# Apply configuration fragments
do_configure:append() {
	# Only accept fragments ending in .cfg. If the fragments contain
	# something other than kernel configs, it will be filtered out
	# automatically.
	if [ -n "${@' '.join(find_cfgs(d))}" ]; then
		${S}/scripts/kconfig/merge_config.sh -m -O ${B} ${B}/.config ${@" ".join(find_cfgs(d))}
	fi
	# Apply ST-specific config fragments (ending in .config and stored in a different folder)
	if [ ! -z "${KERNEL_CONFIG_FRAGMENTS}" ]; then
		for f in ${KERNEL_CONFIG_FRAGMENTS}
		do
			# Check if the config fragment was copied into the WORKDIR from
			# the OE meta data
			if [ ! -e "$f" ]; then
				bb_warn "Could not find kernel config fragment $f"
				exit 1
			fi
		done
		# Now that all the fragments are located merge them.
		(${S}/scripts/kconfig/merge_config.sh -m -r -O ${B} ${B}/.config ${KERNEL_CONFIG_FRAGMENTS} 1>&2 )
	fi
}

# Don't create custom folder for kernel artifacts
do_deploy[sstate-outputdirs] = "${DEPLOY_DIR_IMAGE}"
