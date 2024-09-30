# Copyright (C) 2022-2024, Digi International Inc.

require recipes-kernel/linux/linux-dey.inc

SRCBRANCH = "v5.15/nxp/dey-4.0/maint"
SRCBRANCH:stm32mpcommon = "v5.15/stm/dey-4.0/maint"
SRCREV = "85ab27e1eba8252b78dcf36293c20e49f1d141c6"
SRCREV:stm32mpcommon = "35bde2fa4824c3fc87971a94b188e1f063d7cdf6"

STM_RT_PATCHES = " \
	file://patch-5.15.119-rt65.patch \
	file://0023-5.15-stm32mp-rt-49-r1-CLOCK.patch \
	file://0024-5.15-stm32mp-rt-49-r1-DMA.patch \
	file://0025-5.15-stm32mp-rt-49-r1-MFD.patch \
	file://0026-5.15-stm32mp-rt-49-r1-NET-TTY.patch \
	file://0027-5.15-stm32mp-rt-49-r1-DEVICETREE.patch \
	file://0028-5.15-stm32mp-rt-49-r1-CONFIG.patch \
"

SRC_URI:append:stm32mpcommon = " \
	${@bb.utils.contains('DISTRO_FEATURES', 'rt', '${STM_RT_PATCHES}', '', d)} \
"

KERNEL_CONFIG_FRAGMENTS:append:stm32mpcommon = " ${@bb.utils.contains('DISTRO_FEATURES', 'rt', '${S}/arch/arm/configs/fragment-07-rt.config', '', d)}"
KERNEL_CONFIG_FRAGMENTS:append:stm32mpcommon = " ${@bb.utils.contains('DISTRO_FEATURES', 'rt', '${S}/arch/arm/configs/fragment-07-rt-sysvinit.config', '', d)}"
KERNEL_CONFIG_FRAGMENTS:append:ccmp13 = " ${@bb.utils.contains('DISTRO_FEATURES', 'rt', '${S}/arch/arm/configs/fragment-08-rt-mp13.config', '', d)}"

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
	if [ "${UBOOT_SIGN_ENABLE}" = "1" ] && \
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
