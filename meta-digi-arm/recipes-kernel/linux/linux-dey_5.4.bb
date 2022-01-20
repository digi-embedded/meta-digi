# Copyright (C) 2013-2022 Digi International

SUMMARY = "Linux kernel for Digi boards"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=bbea815ee2795b2f4230826c0c6b8814"

DEPENDS += "lzop-native bc-native"

inherit kernel
inherit ${@oe.utils.conditional('DEY_BUILD_PLATFORM', 'NXP', 'fsl-kernel-localversion', '', d)}

SRCBRANCH = "v5.4.70/master"
require recipes-kernel/linux/linux-dey-src.inc
require ${@bb.utils.contains('DISTRO_FEATURES', 'virtualization', 'linux-virtualization.inc', '', d)}
require recipes-kernel/linux/linux-trustfence.inc

# Use custom provided 'defconfig' if variable KERNEL_DEFCONFIG is cleared
SRC_URI += "${@oe.utils.conditional('KERNEL_DEFCONFIG', '', 'file://defconfig', '', d)}"

FILES_${KERNEL_PACKAGE_NAME}-image += "/boot/config-${KERNEL_VERSION}"

# Don't include kernels in standard images
RDEPENDS_${KERNEL_PACKAGE_NAME}-base = ""

# A user can provide his own kernel 'defconfig' file by:
# - setting the variable KERNEL_DEFCONFIG to a custom kernel configuration file
#   inside the kernel repository.
# - setting the variable KERNEL_DEFCONFIG to a kernel configuration file using
#   the full path to the file.
# - clearing the variable KERNEL_DEFCONFIG and providing a kernel configuration
#   file in the layer (in this case the file must be named 'defconfig').
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

# Apply configuration fragments
do_configure_append() {
	# Only accept fragments ending in .cfg. If the fragments contain
	# something other than kernel configs, it will be filtered out
	# automatically.
	if [ -n "${@' '.join(find_cfgs(d))}" ]; then
		${S}/scripts/kconfig/merge_config.sh -m -O ${B} ${B}/.config ${@" ".join(find_cfgs(d))}
	fi
}

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8x|ccimx8m|ccimx6)"
