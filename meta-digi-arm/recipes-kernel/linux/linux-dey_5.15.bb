# Copyright (C) 2022-2024 Digi International

require recipes-kernel/linux/linux-dey.inc

SRCBRANCH = "v5.15.71/nxp/master"
SRCBRANCH:stm32mpcommon = "v5.15.118/stm/master"
SRCREV = "${AUTOREV}"
SRCREV:stm32mpcommon = "${AUTOREV}"

do_assemble_fitimage:prepend:ccmp1() {
	# Deploy u-boot script to be included into the FIT image
	install -d ${STAGING_DIR_HOST}/boot
	install -m 0644 ${RECIPE_SYSROOT}/${datadir}/${UBOOT_ENV_BINARY} ${STAGING_DIR_HOST}/boot/
}

do_assemble_fitimage:append:ccmp1() {
	#
	# Step 9: Add public keys to the different U-Boot dtb files
	#
	if [ "${UBOOT_SIGN_ENABLE}" = "1" ] && [ -n "${UBOOT_DEVICETREE}" ]; then
		for devicetree in ${UBOOT_DEVICETREE}; do
			if [ -f "${STAGING_DATADIR}/${devicetree}.dtb" ]; then
				cp -P "${STAGING_DATADIR}/${devicetree}.dtb" ${B}

				# Add image public key in U-Boot dtb file
				fdt_add_pubkey -a "${FIT_HASH_ALG},${FIT_SIGN_ALG}" \
							   -k "${UBOOT_SIGN_KEYDIR}" \
							   -n "${UBOOT_SIGN_IMG_KEYNAME}" \
							   -r "image" \
							   "${B}/${devicetree}.dtb"

				# Add configuration public key in U-Boot dtb file
				fdt_add_pubkey -a "${FIT_HASH_ALG},${FIT_SIGN_ALG}" \
							   -k "${UBOOT_SIGN_KEYDIR}" \
							   -n "${UBOOT_SIGN_KEYNAME}" \
							   -r "conf" \
							   "${B}/${devicetree}.dtb"
			fi
		done
	fi
}

kernel_do_deploy:append:ccmp1() {
	if [ "${UBOOT_SIGN_ENABLE}" = "1" -o "${UBOOT_FITIMAGE_ENABLE}" = "1" ] && \
	   [ -n "${UBOOT_DTB_BINARY}" ] ; then
		# Install device tree files with signature
		if [ -n "${UBOOT_DEVICETREE}" ]; then
			for devicetree in ${UBOOT_DEVICETREE}; do
				if [ -f "${B}/${devicetree}.dtb" ]; then
					install -m 0644 ${B}/${devicetree}.dtb "${DEPLOYDIR}/${FIP_UBOOT_DTB}-${devicetree}-with-signature.dtb"
				fi
			done
		fi
	fi
}

COMPATIBLE_MACHINE = "(ccimx6|ccimx6ul|ccimx8m|ccimx8x|ccmp1)"
