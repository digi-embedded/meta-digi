# Copyright (C) 2022 Digi International

LICENSE = "GPLv2"

DEPENDS += "${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'trustfence-sign-tools-native', '', d)}"

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

