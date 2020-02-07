# Copyright (C) 2013-2018 Digi International

SUMMARY = "Linux kernel for Digi boards"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=d7810fab7487fb0aad327b76f1be7cd7"

DEPENDS += "lzop-native bc-native"
DEPENDS += "${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', \
		oe.utils.conditional('TRUSTFENCE_SIGN_MODE', 'AHAB', 'trustfence-sign-tools-native imx-mkimage', 'trustfence-sign-tools-native', d), '', d)}"

inherit kernel fsl-kernel-localversion

require recipes-kernel/linux/linux-dey-src.inc
require ${@bb.utils.contains('DISTRO_FEATURES', 'virtualization', 'linux-virtualization.inc', '', d)}

# Use custom provided 'defconfig' if variable KERNEL_DEFCONFIG is cleared
SRC_URI += "${@oe.utils.conditional('KERNEL_DEFCONFIG', '', 'file://defconfig', '', d)}"

do_deploy[postfuncs] += "${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'trustfence_sign', '', d)}"

trustfence_sign() {
	# Set environment variables for trustfence configuration
	export CONFIG_SIGN_KEYS_PATH="${TRUSTFENCE_SIGN_KEYS_PATH}"
	[ -n "${TRUSTFENCE_KEY_INDEX}" ] && export CONFIG_KEY_INDEX="${TRUSTFENCE_KEY_INDEX}"
	[ -n "${TRUSTFENCE_DEK_PATH}" ] && [ "${TRUSTFENCE_DEK_PATH}" != "0" ] && export CONFIG_DEK_PATH="${TRUSTFENCE_DEK_PATH}"
	[ -n "${TRUSTFENCE_SIGN_MODE}" ] && export CONFIG_SIGN_MODE="${TRUSTFENCE_SIGN_MODE}"

	# Sign/encrypt the kernel images
	if [ "${TRUSTFENCE_SIGN_MODE}" = "HAB" ]; then
		for type in ${KERNEL_IMAGETYPES}; do
			KERNEL_IMAGE="${type}-${KERNEL_IMAGE_NAME}.bin"
			TMP_KERNEL_IMAGE_SIGNED="$(mktemp ${KERNEL_IMAGE}-signed.XXXXXX)"
			trustfence-sign-kernel.sh -p "${DIGI_FAMILY}" -l "${KERNEL_IMAGE}" "${TMP_KERNEL_IMAGE_SIGNED}"
			mv "${TMP_KERNEL_IMAGE_SIGNED}" "${KERNEL_IMAGE}"
		done

		# Sign/encrypt the device tree blobs
		for DTB in ${KERNEL_DEVICETREE}; do
			DTB=`normalize_dtb "${DTB}"`
			DTB_EXT=${DTB##*.}
			DTB_BASE_NAME=`basename ${DTB} ."${DTB_EXT}"`
			DTB_IMAGE="${DTB_BASE_NAME}-${KERNEL_IMAGE_NAME}.${DTB_EXT}"
			TMP_DTB_IMAGE_SIGNED="$(mktemp ${DTB_IMAGE}-signed.XXXXXX)"
			trustfence-sign-kernel.sh -p "${DIGI_FAMILY}" -d "${DTB_IMAGE}" "${TMP_DTB_IMAGE_SIGNED}"
			mv "${TMP_DTB_IMAGE_SIGNED}" "${DTB_IMAGE}"
		done
	elif [ "${TRUSTFENCE_SIGN_MODE}" = "AHAB" ]; then
		# Sign the kernel images
		for type in ${KERNEL_IMAGETYPES}; do
			KERNEL_IMAGE="${type}-${KERNEL_IMAGE_NAME}.bin"
			${DEPLOY_DIR_IMAGE}/imx-boot-tools/mkimage_imx8 -soc ${MX8_SOC_VAR} -rev ${MX8_CHIP_REV} -c -ap ${WORKDIR}/build/arch/arm64/boot/Image a35 ${RAM_CONTAINER_LOC_BOOT} -out flash_os.bin
			trustfence-sign-kernel.sh -p "${DIGI_FAMILY}" -l "flash_os.bin" "${type}-${MACHINE}-signed.bin"
			gzip ${type}-${MACHINE}-signed.bin
			mv ${type}-${MACHINE}-signed.bin.gz "${KERNEL_IMAGE}"
		done

		# Sign/encrypt the device tree blobs
		for DTB in ${KERNEL_DEVICETREE}; do
			DTB=`normalize_dtb "${DTB}"`
			DTB_EXT=${DTB##*.}
			DTB_BASE_NAME=`basename ${DTB} ."${DTB_EXT}"`
			DTB_IMAGE="${DTB_BASE_NAME}-${KERNEL_IMAGE_NAME}.${DTB_EXT}"
			${DEPLOY_DIR_IMAGE}/imx-boot-tools/mkimage_imx8 -soc ${MX8_SOC_VAR} -rev ${MX8_CHIP_REV} -c -ap ${DTB_IMAGE} a35 ${RAM_CONTAINER_LOC_DTB} -out ${DTB_IMAGE}-mkimg-signed
			trustfence-sign-kernel.sh -p "${DIGI_FAMILY}" -d "${DTB_IMAGE}-mkimg-signed" "${DTB_IMAGE}-signed"
			mv "${DTB_IMAGE}-signed" "${DTB_IMAGE}"
			rm -f ${DTB_IMAGE}-mkimg-signed
		done
	else
		bberror "Unkown TRUSTFENCE_SIGN_MODE value"
		exit 1
	fi
}
trustfence_sign[dirs] = "${DEPLOYDIR}"

do_deploy[vardeps] += "TRUSTFENCE_SIGN_KEYS_PATH TRUSTFENCE_KEY_INDEX TRUSTFENCE_DEK_PATH"

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
addtask copy_defconfig after do_patch before do_preconfigure
