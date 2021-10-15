# Copyright (C) 2013-2020 Digi International

SUMMARY = "Linux kernel for Digi boards"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=d7810fab7487fb0aad327b76f1be7cd7"

DEPENDS += "lzop-native bc-native"
DEPENDS += "${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'trustfence-sign-tools-native', '', d)}"

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
	for type in ${KERNEL_IMAGETYPES}; do
		KERNEL_IMAGE="${type}-${KERNEL_IMAGE_NAME}.bin"
		if [ "${type}" = "Image.gz" ]; then
			# Sign the uncompressed Image
			KERNEL_IMAGE=${WORKDIR}/build/arch/arm64/boot/Image
		fi

		TMP_KERNEL_IMAGE_SIGNED="$(mktemp ${KERNEL_IMAGE}-signed.XXXXXX)"
		trustfence-sign-artifact.sh -p "${DIGI_FAMILY}" -l "${KERNEL_IMAGE}" "${TMP_KERNEL_IMAGE_SIGNED}"

		if [ "${type}" = "Image.gz" ]; then
			# Compress the signed Image and restore the original filename
			gzip "${TMP_KERNEL_IMAGE_SIGNED}"
			mv "${TMP_KERNEL_IMAGE_SIGNED}.gz" "${TMP_KERNEL_IMAGE_SIGNED}"
			KERNEL_IMAGE="${type}-${KERNEL_IMAGE_NAME}.bin"
		fi

		mv "${TMP_KERNEL_IMAGE_SIGNED}" "${KERNEL_IMAGE}"
	done

	# Sign/encrypt the device tree blobs
	for DTB in ${KERNEL_DEVICETREE}; do
		DTB=`normalize_dtb "${DTB}"`
		DTB_EXT=${DTB##*.}
		DTB_BASE_NAME=`basename ${DTB} ."${DTB_EXT}"`
		DTB_IMAGE="${DTB_BASE_NAME}-${KERNEL_IMAGE_NAME}.${DTB_EXT}"

		TMP_DTB_IMAGE_SIGNED="$(mktemp ${DTB_IMAGE}-signed.XXXXXX)"
		if [ "${DTB_EXT}" = "dtbo" ]; then
			trustfence-sign-artifact.sh -p "${DIGI_FAMILY}" -o "${DTB_IMAGE}" "${TMP_DTB_IMAGE_SIGNED}"
		else
			trustfence-sign-artifact.sh -p "${DIGI_FAMILY}" -d "${DTB_IMAGE}" "${TMP_DTB_IMAGE_SIGNED}"
		fi
		mv "${TMP_DTB_IMAGE_SIGNED}" "${DTB_IMAGE}"
	done
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
