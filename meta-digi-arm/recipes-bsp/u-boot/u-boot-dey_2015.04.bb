# Copyright (C) 2012-2015 Digi International

require recipes-bsp/u-boot/u-boot.inc

DESCRIPTION = "Bootloader for Digi platforms"
LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://Licenses/README;md5=c7383a594871c03da76b3707929d2919"

DEPENDS += "dtc-native u-boot-mkimage-native"

PROVIDES += "u-boot"

SRCBRANCH = "v2015.04/master"
SRCREV = "${AUTOREV}"

# Select internal or Github U-Boot repo
UBOOT_GIT_URI = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${DIGI_GIT}u-boot-denx.git', '${DIGI_GITHUB_GIT}/u-boot.git', d)}"

SRC_URI = " \
    ${UBOOT_GIT_URI};branch=${SRCBRANCH} \
    file://boot.txt \
    file://install_linux_fw_sd.txt \
"

LOCALVERSION ?= ""
inherit fsl-u-boot-localversion

EXTRA_OEMAKE_append = " KCFLAGS=-fgnu89-inline"

do_deploy_append() {
	# Remove canonical U-Boot symlinks for ${UBOOT_CONFIG} currently in the form:
	#    u-boot-<platform>.imx-<type>
	#    u-boot-<type>
	# and add a more suitable symlink in the form:
	#    u-boot-<platform>-<config>.imx
	if [ "x${UBOOT_CONFIG}" != "x" ]; then
		for config in ${UBOOT_MACHINE}; do
			i=`expr $i + 1`
			for type in ${UBOOT_CONFIG}; do
				j=`expr $j + 1`
				if [ $j -eq $i ]; then
					cd ${DEPLOYDIR}
					rm -r ${UBOOT_BINARY}-${type} ${UBOOT_SYMLINK}-${type}
					ln -sf u-boot-${type}-${PV}-${PR}.${UBOOT_SUFFIX} u-boot-${type}.${UBOOT_SUFFIX}
				fi
			done
			unset  j
		done
		unset  i
	fi

	# DEY firmware install script
	sed -i -e 's,##GRAPHICAL_BACKEND##,${GRAPHICAL_BACKEND},g' ${WORKDIR}/install_linux_fw_sd.txt
	mkimage -T script -n "DEY firmware install script" -C none -d ${WORKDIR}/install_linux_fw_sd.txt ${DEPLOYDIR}/install_linux_fw_sd.scr

	# Boot script for DEY images
	mkimage -T script -n bootscript -C none -d ${WORKDIR}/boot.txt ${DEPLOYDIR}/boot.scr
}

COMPATIBLE_MACHINE = "(ccimx6)"
