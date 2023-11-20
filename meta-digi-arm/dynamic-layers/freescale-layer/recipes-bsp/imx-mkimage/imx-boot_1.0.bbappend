# Copyright (C) 2022-2023 Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append:ccimx8m = " \
    file://0001-imx8m-soc.mak-preserve-dtbs-after-build.patch \
    file://0002-imx8m-soc.mak-capture-commands-output-into-a-log-fil.patch \
"

SRC_URI:append:ccimx93 = " \
    file://0001-imx9-soc.mak-capture-commands-output-into-a-log-file.patch \
    file://0002-imx9-soc.mak-add-makefile-target-to-build-A0-revisio.patch \
"

# Use NXP's lf-6.1.36-2.1.0 release for ccimx93
SRC_URI:ccimx93 = "git://github.com/nxp-imx/imx-mkimage.git;protocol=https;branch=${SRCBRANCH}"
SRCBRANCH:ccimx93 = "lf-6.1.36_2.1.0"
SRCREV:ccimx93 = "5a0faefc223e51e088433663b6e7d6fbce89bf59"

DEPENDS += "${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'trustfence-sign-tools-native', '', d)}"

# Do not tag imx-boot
UUU_BOOTLOADER = ""
UUU_BOOTLOADER_TAGGED = ""

REV_OPTION:ccimx93 = "REV=A1"

compile_mx8m:append:ccimx8m() {
	# Create dummy DEK blob to support building with encrypted u-boot
	if [ -n "${TRUSTFENCE_DEK_PATH}" ] && [ "${TRUSTFENCE_DEK_PATH}" != "0" ]; then
		dd if=/dev/zero of=${BOOT_STAGING}/dek_blob_fit_dummy.bin bs=96 count=1 oflag=sync
	fi
}

# For SOC revision A0 we need a different ATF binary
compile_mx93:append:ccimx93() {
	if [ "$target" = "flash_singleboot_a0" ]; then
		ATF_MACHINE_NAME_A0="$(echo ${ATF_MACHINE_NAME} | sed -e 's,.bin,-A0.bin,g')"
		bbnote "Copy ATF binary for SOC revision A0: ${ATF_MACHINE_NAME_A0}"
		\cp --remove-destination ${DEPLOY_DIR_IMAGE}/${ATF_MACHINE_NAME_A0} ${BOOT_STAGING}/bl31.bin
		unset ATF_MACHINE_NAME_A0
	fi
}

do_compile:append:ccimx8m() {
	bbnote "building ${IMX_BOOT_SOC_TARGET} - print_fit_hab"
	make SOC=${IMX_BOOT_SOC_TARGET} dtbs=${UBOOT_DTB_NAME} print_fit_hab
}

do_compile:ccimx8x() {
	# Copy TEE binary to SoC target folder to mkimage
	if ${DEPLOY_OPTEE}; then
		cp ${DEPLOY_DIR_IMAGE}/tee.bin ${BOOT_STAGING}
	fi
	UBOOT_CONFIG_EXTRA="${UBOOT_CONFIG}"
	UBOOT_NAME_EXTRA="u-boot-${MACHINE}.bin-${UBOOT_CONFIG_EXTRA}"
	for target in ${IMXBOOT_TARGETS}; do
		compile_${SOC_FAMILY}
		for rev in ${SOC_REVISIONS}; do
			bbnote "building ${IMX_BOOT_SOC_TARGET} - REV=${rev} ${target}"
			make SOC=${IMX_BOOT_SOC_TARGET} REV=${rev} ${target} > ${S}/mkimage-${rev}-${target}.log 2>&1
			if [ -e "${BOOT_STAGING}/flash.bin" ]; then
				cp ${BOOT_STAGING}/flash.bin ${S}/${BOOT_NAME}-${MACHINE}-${rev}.bin-${target}
			fi
			# Remove u-boot-atf-container.img so it gets generated in the next iteration
			rm ${BOOT_STAGING}/u-boot-atf-container.img
		done
	done
	unset UBOOT_CONFIG_EXTRA
	unset UBOOT_NAME_EXTRA
}

do_install:ccimx8x () {
	install -d ${D}/boot
	for bin in ${BOOTABLE_ARTIFACTS}; do
		for target in ${IMXBOOT_TARGETS}; do
			install -m 0644 ${S}/${bin}-${target} ${D}/boot/
		done
	done
}

generate_symlinks() {
	# imx-boot recipe in meta-freescale supports *multiple* build configurations.
	# We assume here only ONE build configuration for our platforms (otherwise
	# UBOOT_CONFIG would be incorrectly expanded)
	for target in ${IMXBOOT_TARGETS}; do
		mv ${DEPLOYDIR}/${BOOT_NAME}-${MACHINE}-${UBOOT_CONFIG}.bin-${target} ${DEPLOYDIR}/${BOOT_NAME}-${MACHINE}.bin-${target}
	done
	ln -sf ${BOOT_NAME}-${MACHINE}.bin-${IMAGE_IMXBOOT_TARGET} ${DEPLOYDIR}/${BOOT_NAME}-${MACHINE}.bin
	ln -sf ${BOOT_NAME}-${MACHINE}.bin-${IMAGE_IMXBOOT_TARGET} ${DEPLOYDIR}/${BOOT_NAME}
}

do_deploy:append:ccimx8m() {
	generate_symlinks
	for target in ${IMXBOOT_TARGETS}; do
		install -m 0644 ${BOOT_STAGING}/mkimage-${target}.log ${DEPLOYDIR}/${BOOT_TOOLS}
	done
	install -m 0644 ${BOOT_STAGING}/mkimage-print_fit_hab.log ${DEPLOYDIR}/${BOOT_TOOLS}
}

do_deploy:append:ccimx93() {
	generate_symlinks
	for target in ${IMXBOOT_TARGETS}; do
		install -m 0644 ${BOOT_STAGING}/mkimage-${target}.log ${DEPLOYDIR}/${BOOT_TOOLS}
		# Generate symlink for SOC revision A0
		if [ "$target" = "flash_singleboot_a0" ]; then
			ln -sf ${BOOT_NAME}-${MACHINE}.bin-${target} ${DEPLOYDIR}/${BOOT_NAME}-${MACHINE}-A0.bin
		fi
	done
}

do_deploy:ccimx8x () {
	deploy_${SOC_FAMILY}
	# copy tee.bin to deploy path
	if "${DEPLOY_OPTEE}"; then
		install -m 0644 ${DEPLOY_DIR_IMAGE}/tee.bin ${DEPLOYDIR}/${BOOT_TOOLS}
	fi
	# copy makefile (soc.mak) for reference
	install -m 0644 ${BOOT_STAGING}/soc.mak ${DEPLOYDIR}/${BOOT_TOOLS}
	# copy the generated boot image to deploy path
	for rev in ${SOC_REVISIONS}; do
		IMAGE_IMXBOOT_TARGET=""
		for target in ${IMXBOOT_TARGETS}; do
			# Use first "target" as IMAGE_IMXBOOT_TARGET
			if [ "$IMAGE_IMXBOOT_TARGET" = "" ]; then
				IMAGE_IMXBOOT_TARGET="$target"
				echo "Set boot target as $IMAGE_IMXBOOT_TARGET"
			fi
			install -m 0644 ${S}/${UBOOT_PREFIX}-${MACHINE}-${rev}.bin-${target} ${DEPLOYDIR}
			# copy make log for reference
			install -m 0644 ${S}/mkimage-${rev}-${target}.log ${DEPLOYDIR}/${BOOT_TOOLS}
		done
		cd ${DEPLOYDIR}
		ln -sf ${UBOOT_PREFIX}-${MACHINE}-${rev}.bin-${IMAGE_IMXBOOT_TARGET} ${UBOOT_PREFIX}-${MACHINE}-${rev}.bin
		# Link to default bootable U-Boot filename. It gets overwritten
		# on every loop so the only last RAM_CONFIG will survive.
		ln -sf ${UBOOT_PREFIX}-${MACHINE}-${rev}.bin-${IMAGE_IMXBOOT_TARGET} ${BOOTABLE_FILENAME}
		cd -
	done
}

do_deploy[postfuncs] += "${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'trustfence_sign_imxboot', '', d)}"
trustfence_sign_imxboot() {
	TF_SIGN_ENV="CONFIG_SIGN_KEYS_PATH=${TRUSTFENCE_SIGN_KEYS_PATH}"
	TF_SIGN_ENV="$TF_SIGN_ENV CONFIG_FIT_HAB_LOG_PATH=${DEPLOYDIR}/${BOOT_TOOLS}/mkimage-print_fit_hab.log"
	[ -n "${TRUSTFENCE_KEY_INDEX}" ] && TF_SIGN_ENV="$TF_SIGN_ENV CONFIG_KEY_INDEX=${TRUSTFENCE_KEY_INDEX}"
	[ -n "${TRUSTFENCE_SIGN_MODE}" ] && TF_SIGN_ENV="$TF_SIGN_ENV CONFIG_SIGN_MODE=${TRUSTFENCE_SIGN_MODE}"
	[ -n "${TRUSTFENCE_SRK_REVOKE_MASK}" ] && TF_SIGN_ENV="$TF_SIGN_ENV SRK_REVOKE_MASK=${TRUSTFENCE_SRK_REVOKE_MASK}"
	[ -n "${TRUSTFENCE_UNLOCK_KEY_REVOCATION}" ] && TF_SIGN_ENV="$TF_SIGN_ENV CONFIG_UNLOCK_SRK_REVOKE=${TRUSTFENCE_UNLOCK_KEY_REVOCATION}"

	# Sign/encrypt boot image
	for target in ${IMXBOOT_TARGETS}; do
		TF_SIGN_ENV="$TF_SIGN_ENV CONFIG_MKIMAGE_LOG_PATH=${DEPLOYDIR}/${BOOT_TOOLS}/mkimage-${target}.log"
		env $TF_SIGN_ENV trustfence-sign-uboot.sh ${BOOT_NAME}-${MACHINE}.bin-${target} ${BOOT_NAME}-signed-${MACHINE}.bin-${target}
		if [ -n "${TRUSTFENCE_DEK_PATH}" ] && [ "${TRUSTFENCE_DEK_PATH}" != "0" ]; then
			TF_ENC_ENV="CONFIG_DEK_PATH=${TRUSTFENCE_DEK_PATH} ENABLE_ENCRYPTION=y"
			env $TF_SIGN_ENV $TF_ENC_ENV trustfence-sign-uboot.sh ${BOOT_NAME}-${MACHINE}.bin-${target} ${BOOT_NAME}-encrypted-${MACHINE}.bin-${target}
		fi
	done
}

trustfence_sign_imxboot:ccimx8x() {
	TF_SIGN_ENV="CONFIG_SIGN_KEYS_PATH=${TRUSTFENCE_SIGN_KEYS_PATH}"
	[ -n "${TRUSTFENCE_KEY_INDEX}" ] && TF_SIGN_ENV="$TF_SIGN_ENV CONFIG_KEY_INDEX=${TRUSTFENCE_KEY_INDEX}"
	[ -n "${TRUSTFENCE_SIGN_MODE}" ] && TF_SIGN_ENV="$TF_SIGN_ENV CONFIG_SIGN_MODE=${TRUSTFENCE_SIGN_MODE}"
	[ -n "${TRUSTFENCE_SRK_REVOKE_MASK}" ] && TF_SIGN_ENV="$TF_SIGN_ENV SRK_REVOKE_MASK=${TRUSTFENCE_SRK_REVOKE_MASK}"

	# Sign/encrypt boot image
	for target in ${IMXBOOT_TARGETS}; do
		for rev in ${SOC_REVISIONS}; do
			TF_SIGN_ENV="$TF_SIGN_ENV CONFIG_MKIMAGE_LOG_PATH=${DEPLOYDIR}/${BOOT_TOOLS}/mkimage-${rev}-${target}.log"
			env $TF_SIGN_ENV trustfence-sign-uboot.sh ${BOOT_NAME}-${MACHINE}-${rev}.bin-${target} ${BOOT_NAME}-signed-${MACHINE}-${rev}.bin-${target}
			if [ -n "${TRUSTFENCE_DEK_PATH}" ] && [ "${TRUSTFENCE_DEK_PATH}" != "0" ]; then
				TF_ENC_ENV="CONFIG_DEK_PATH=${TRUSTFENCE_DEK_PATH} ENABLE_ENCRYPTION=y"
				env $TF_SIGN_ENV $TF_ENC_ENV trustfence-sign-uboot.sh ${BOOT_NAME}-${MACHINE}-${rev}.bin-${target} ${BOOT_NAME}-encrypted-${MACHINE}-${rev}.bin-${target}
			fi
		done
	done
}

trustfence_sign_imxboot[dirs] = "${DEPLOYDIR}"
trustfence_sign_imxboot[vardeps] += "TRUSTFENCE_SIGN_KEYS_PATH TRUSTFENCE_KEY_INDEX TRUSTFENCE_DEK_PATH TRUSTFENCE_SIGN_MODE TRUSTFENCE_SRK_REVOKE_MASK TRUSTFENCE_UNLOCK_KEY_REVOCATION"
