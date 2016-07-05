# Copyright (C) 2013 Digi International

SUMMARY = "Linux kernel for Digi boards"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=d7810fab7487fb0aad327b76f1be7cd7"

DEPENDS += "lzop-native bc-native"
DEPENDS += "${@base_conditional('TRUSTFENCE_SIGN', '1', 'trustfence-cst-native', '', d)}"

inherit kernel fsl-kernel-localversion

LOCALVERSION = "-dey"

# Select internal or Github Linux repo
LINUX_GIT_URI = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${DIGI_GIT}linux-2.6.git', '${DIGI_GITHUB_GIT}/linux.git', d)}"

SRC_URI = " \
    ${LINUX_GIT_URI};branch=${SRCBRANCH} \
    file://defconfig \
"

S = "${WORKDIR}/git"

# We need to pass it as param since kernel might support more than one
# machine, with different entry points
KERNEL_EXTRA_ARGS += "LOADADDR=${UBOOT_ENTRYPOINT}"

# In order to sign and encrypt the uImage, we need to know the address
# in which U-Boot loads the uImage. This is CONFIG_LOADADDR in U-Boot
# configuration file. 
UBOOT_CONFIG_LOADADDR = ""
UBOOT_CONFIG_LOADADDR_ccimx6 = "0x12000000"
UBOOT_CONFIG_LOADADDR_ccimx6ul = "0x80800000"

do_deploy_append() {
	if [ -n "${TRUSTFENCE_SIGN}" ]; then
		export CONFIG_SIGN_KEYS_PATH="${TRUSTFENCE_SIGN_KEYS_PATH}"
		export CONFIG_UIMAGE_LOADADDR="${UBOOT_CONFIG_LOADADDR}"
		[ -n "${TRUSTFENCE_KEY_INDEX}" ] && export CONFIG_KEY_INDEX="${TRUSTFENCE_KEY_INDEX}"
		[ -n "${TRUSTFENCE_DEK_PATH}" ] && [ "${TRUSTFENCE_DEK_PATH}" != "0" ] && export CONFIG_DEK_PATH="${TRUSTFENCE_DEK_PATH}"
		"${THISDIR}/../../scripts/trustfence-sign-uimage.sh" "${DEPLOYDIR}/${KERNEL_IMAGE_BASE_NAME}.bin" "${DEPLOYDIR}/${KERNEL_IMAGE_BASE_NAME}-signed.bin"
		mv "${DEPLOYDIR}/${KERNEL_IMAGE_BASE_NAME}-signed.bin" "${DEPLOYDIR}/${KERNEL_IMAGE_BASE_NAME}.bin"
	fi	
	(cd ${DEPLOYDIR} && ln -sf ${KERNEL_IMAGE_BASE_NAME}.bin ${KERNEL_IMAGE_SYMLINK_NAME})
}

FILES_kernel-image += "/boot/config-${KERNEL_VERSION}"

# Don't include kernels in standard images
RDEPENDS_kernel-base = ""
