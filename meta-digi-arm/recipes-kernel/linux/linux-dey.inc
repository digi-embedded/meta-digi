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
LINUX_GIT_URI = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${DIGI_GIT}linux-2.6.git', '${DIGI_GITHUB_GIT}/linux.git', d)}"

SRC_URI = " \
    ${LINUX_GIT_URI};branch=${SRCBRANCH} \
    file://defconfig \
"

S = "${WORKDIR}/git"

# We need to pass it as param since kernel might support more than one
# machine, with different entry points
KERNEL_EXTRA_ARGS += "LOADADDR=${UBOOT_ENTRYPOINT}"

do_deploy_append() {
	if [ "${TRUSTFENCE_SIGN}" = "1" ]; then
		# Set environment variables for trustfence configuration
		export CONFIG_SIGN_KEYS_PATH="${TRUSTFENCE_SIGN_KEYS_PATH}"
		[ -n "${TRUSTFENCE_KEY_INDEX}" ] && export CONFIG_KEY_INDEX="${TRUSTFENCE_KEY_INDEX}"
		[ -n "${TRUSTFENCE_DEK_PATH}" ] && [ "${TRUSTFENCE_DEK_PATH}" != "0" ] && export CONFIG_DEK_PATH="${TRUSTFENCE_DEK_PATH}"

		# Sign/encrypt the kernel image
		"${STAGING_BINDIR_NATIVE}/trustfence-sign-kernel.sh" -p "${DIGI_FAMILY}" -l "${DEPLOYDIR}/${KERNEL_IMAGE_BASE_NAME}.bin" "${DEPLOYDIR}/${KERNEL_IMAGE_BASE_NAME}-signed.bin"
		mv "${DEPLOYDIR}/${KERNEL_IMAGE_BASE_NAME}-signed.bin" "${DEPLOYDIR}/${KERNEL_IMAGE_BASE_NAME}.bin"

		# Sign/encrypt the device tree blobs
		if [ -n "${KERNEL_DEVICETREE}" ]; then
			for DTB_NAME in ${KERNEL_DEVICETREE}; do
				DTB="${B}/arch/${ARCH}/boot/dts/${DTB_NAME}"
				"${STAGING_BINDIR_NATIVE}/trustfence-sign-kernel.sh" -p "${DIGI_FAMILY}" -d "${DTB}" "${DTB}-signed"
				mv "${DTB}-signed" "${DTB}"
			done
		fi
	fi
	(cd ${DEPLOYDIR} && ln -sf ${KERNEL_IMAGE_BASE_NAME}.bin ${KERNEL_IMAGE_SYMLINK_NAME})
}

do_deploy[vardeps] += "TRUSTFENCE_SIGN_KEYS_PATH TRUSTFENCE_KEY_INDEX TRUSTFENCE_DEK_PATH"

FILES_kernel-image += "/boot/config-${KERNEL_VERSION}"

# Don't include kernels in standard images
RDEPENDS_kernel-base = ""
