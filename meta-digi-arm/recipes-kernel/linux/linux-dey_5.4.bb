# Copyright (C) 2013-2022 Digi International

SUMMARY = "Linux kernel for Digi boards"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=bbea815ee2795b2f4230826c0c6b8814"

DEPENDS += "lzop-native bc-native"

inherit kernel
inherit ${@oe.utils.conditional('DEY_BUILD_PLATFORM', 'NXP', 'fsl-kernel-localversion', '', d)}

SRCBRANCH = "v5.4.70/master"
SRCREV = "248cd6b2bfbee3f3a21175c0c516ab6fa1289318"

require recipes-kernel/linux/linux-dey-src.inc
require ${@bb.utils.contains('DISTRO_FEATURES', 'virtualization', 'linux-virtualization.inc', '', d)}
require recipes-kernel/linux/linux-trustfence.inc

# Use custom provided 'defconfig' if variable KERNEL_DEFCONFIG is cleared
SRC_URI += "${@oe.utils.conditional('KERNEL_DEFCONFIG', '', 'file://defconfig', '', d)}"

# Apply configuration fragments
do_configure:append() {
	# Only accept fragments ending in .cfg. If the fragments contain
	# something other than kernel configs, it will be filtered out
	# automatically.
	if [ -n "${@' '.join(find_cfgs(d))}" ]; then
		${S}/scripts/kconfig/merge_config.sh -m -O ${B} ${B}/.config ${@" ".join(find_cfgs(d))}
	fi
}

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8x|ccimx8m|ccimx6)"
