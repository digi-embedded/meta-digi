# Copyright (C) 2013 Digi International

SUMMARY = "Linux kernel for Digi boards"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=d7810fab7487fb0aad327b76f1be7cd7"

DEPENDS += "lzop-native bc-native"
DEPENDS += "${@base_conditional('TRUSTFENCE_SIGN', '1', 'trustfence-cst-native', '', d)}"
DEPENDS += "${@base_conditional('TRUSTFENCE_SIGN', '1', 'trustfence-sign-tools-native', '', d)}"

inherit kernel fsl-kernel-localversion

LOCALVERSION = "-dey"

# Select internal or Github Linux repo
DIGI_LOG_REPO = "linux-2.6.git"
DIGI_MTK_REPO = "linux/linux.git"
GITHUB_REPO = "linux.git"
LINUX_GIT_URI ?= "${@base_conditional('DIGI_INTERNAL_GIT', '1' , \
					base_conditional('DIGI_GIT', '${DIGI_LOG_GIT}', '${DIGI_GIT}/${DIGI_LOG_REPO}', '${DIGI_GIT}/${DIGI_MTK_REPO};protocol=ssh', d), \
					'${DIGI_GITHUB_GIT}/${GITHUB_REPO};protocol=https', d)}"

SRC_URI = " \
    ${LINUX_GIT_URI};branch=${SRCBRANCH} \
    file://defconfig \
"

S = "${WORKDIR}/git"

# We need to pass it as param since kernel might support more than one
# machine, with different entry points
KERNEL_EXTRA_ARGS += "LOADADDR=${UBOOT_ENTRYPOINT}"

do_deploy[postfuncs] += "${@base_conditional('TRUSTFENCE_SIGN', '1', 'trustfence_sign', '', d)}"

trustfence_sign() {
	# Set environment variables for trustfence configuration
	export CONFIG_SIGN_KEYS_PATH="${TRUSTFENCE_SIGN_KEYS_PATH}"
	[ -n "${TRUSTFENCE_KEY_INDEX}" ] && export CONFIG_KEY_INDEX="${TRUSTFENCE_KEY_INDEX}"
	[ -n "${TRUSTFENCE_DEK_PATH}" ] && [ "${TRUSTFENCE_DEK_PATH}" != "0" ] && export CONFIG_DEK_PATH="${TRUSTFENCE_DEK_PATH}"

	# Sign/encrypt the kernel images
	for type in ${KERNEL_IMAGETYPES}; do
		KERNEL_IMAGE="${type}-${KERNEL_IMAGE_BASE_NAME}.bin"
		TMP_KERNEL_IMAGE_SIGNED="$(mktemp ${KERNEL_IMAGE}-signed.XXXXXX)"
		trustfence-sign-kernel.sh -p "${DIGI_FAMILY}" -l "${KERNEL_IMAGE}" "${TMP_KERNEL_IMAGE_SIGNED}"
		mv "${TMP_KERNEL_IMAGE_SIGNED}" "${KERNEL_IMAGE}"
	done

	# Sign/encrypt the device tree blobs
	for DTB in ${KERNEL_DEVICETREE}; do
		DTB=`normalize_dtb "${DTB}"`
		DTB_EXT=${DTB##*.}
		DTB_BASE_NAME=`basename ${DTB} ."${DTB_EXT}"`
		for type in ${KERNEL_IMAGETYPES}; do
			DTB_IMAGE="$(echo ${type}-${KERNEL_IMAGE_BASE_NAME} | sed "s/${MACHINE}/${DTB_BASE_NAME}/g").${DTB_EXT}"
			TMP_DTB_IMAGE_SIGNED="$(mktemp ${DTB_IMAGE}-signed.XXXXXX)"
			trustfence-sign-kernel.sh -p "${DIGI_FAMILY}" -d "${DTB_IMAGE}" "${TMP_DTB_IMAGE_SIGNED}"
			mv "${TMP_DTB_IMAGE_SIGNED}" "${DTB_IMAGE}"
		done
	done
}
trustfence_sign[dirs] = "${DEPLOYDIR}"

do_deploy[vardeps] += "TRUSTFENCE_SIGN_KEYS_PATH TRUSTFENCE_KEY_INDEX TRUSTFENCE_DEK_PATH"

FILES_kernel-image += "/boot/config-${KERNEL_VERSION}"

# Don't include kernels in standard images
RDEPENDS_kernel-base = ""
