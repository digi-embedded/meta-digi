# Copyright (C) 2022-2025, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

DEPENDS += "${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'trustfence-sign-tools-native', '', d)}"

SRC_URI:append:dey = " \
    file://0001-iMX8QX-soc.mak-capture-commands-output-into-a-log-fi.patch \
    file://0002-imx8m-soc.mak-capture-commands-output-into-a-log-fil.patch \
    file://0003-imx8m-print_fit_hab-follow-symlinks.patch \
    file://0004-imx8mm-adjust-TEE_LOAD_ADDR-for-ccimx8mm.patch \
    file://0005-imx93-soc.mak-capture-commands-output-into-a-log-fil.patch \
    file://0006-imx93-soc.mak-add-makefile-target-to-build-A0-revisi.patch \
    file://0007-imx91-soc.mak-capture-commands-output-into-a-log-fil.patch \
    file://0008-imx95-soc.mak-capture-commands-output-into-a-log-fil.patch \
"

IMX_CORTEXM_DEMOS = ""
IMX_CORTEXM_DEMOS:ccimx95 = "imx-m7-demos:do_deploy"

CORTEXM_DEFAULT_IMAGE = ""
CORTEXM_DEFAULT_IMAGE:ccimx95 = "imx95-19x19-evk_m7_TCM_power_mode_switch.bin"

do_compile[depends] += "${IMX_CORTEXM_DEMOS}"

compile_mx95:append:ccimx95() {
    cp ${DEPLOY_DIR_IMAGE}/mcore-demos/${CORTEXM_DEFAULT_IMAGE} ${BOOT_STAGING}/m7_image.bin
}

# Revert compile_mx8m() to how it was in kirkstone branch of meta-freescale,
# otherwise, a dead symlink is created in place of the dtb
compile_mx8m() {
    bbnote 8MQ/8MM/8MN/8MP boot binary build
    for ddr_firmware in ${DDR_FIRMWARE_NAME}; do
        bbnote "Copy ddr_firmware: ${ddr_firmware} from ${DEPLOY_DIR_IMAGE} -> ${BOOT_STAGING} "
        cp ${DEPLOY_DIR_IMAGE}/${ddr_firmware}               ${BOOT_STAGING}
    done

    cp ${DEPLOY_DIR_IMAGE}/signed_dp_imx8m.bin               ${BOOT_STAGING}
    cp ${DEPLOY_DIR_IMAGE}/signed_hdmi_imx8m.bin             ${BOOT_STAGING}
    cp ${DEPLOY_DIR_IMAGE}/u-boot-spl.bin-${MACHINE}-${UBOOT_CONFIG_EXTRA} \
                                                             ${BOOT_STAGING}/u-boot-spl.bin

    if [ "x${UBOOT_SIGN_ENABLE}" = "x1" ] ; then
        # Use DTB binary patched with signature node
        cp ${DEPLOY_DIR_IMAGE}/${UBOOT_DTB_BINARY} ${BOOT_STAGING}/${UBOOT_DTB_NAME_EXTRA}
    else
        cp ${DEPLOY_DIR_IMAGE}/${BOOT_TOOLS}/${UBOOT_DTB_NAME_EXTRA}   ${BOOT_STAGING}
    fi

    cp ${DEPLOY_DIR_IMAGE}/${BOOT_TOOLS}/u-boot-nodtb.bin-${MACHINE}-${UBOOT_CONFIG_EXTRA} \
                                                             ${BOOT_STAGING}/u-boot-nodtb.bin

    cp ${DEPLOY_DIR_IMAGE}/${ATF_MACHINE_NAME}               ${BOOT_STAGING}/bl31.bin

    cp ${DEPLOY_DIR_IMAGE}/${UBOOT_NAME_EXTRA}                     ${BOOT_STAGING}/u-boot.bin

}

compile_mx8m:append:ccimx8m() {
	# Create dummy DEK blob to support building with encrypted u-boot
	if [ "${TRUSTFENCE_ENCRYPT}" = "1" ]; then
		dd if=/dev/zero of=${BOOT_STAGING}/dek_blob_fit_dummy.bin bs=96 count=1 oflag=sync
	fi
}

# For SOC revision A0 we need a different ATF binary
compile_mx93:append:ccimx93() {
	if [ "$target" = "flash_singleboot_a0" ]; then
		ATF_MACHINE_NAME_A0="$(echo ${ATF_MACHINE_NAME} | sed -e 's,.bin,-A0.bin,g')"
		bbnote "Copy ATF binary for SOC revision A0: ${ATF_MACHINE_NAME_A0}"
		\cp --remove-destination ${DEPLOY_DIR_IMAGE}/${ATF_MACHINE_NAME_A0} ${BOOT_STAGING}/bl31.bin
		# Filename must match the deployed one in "optee-os" recipe for A0 SOC revision
		\cp --remove-destination ${DEPLOY_DIR_IMAGE}/tee.ccimx93dvk_a0.bin ${BOOT_STAGING}/tee.bin
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
				cp ${BOOT_STAGING}/flash.bin ${S}/imx-boot-${MACHINE}-${rev}.bin-${target}
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
	# Remove ##SIGNED## placeholder from variable (signing takes place later)
	BOOT_ARTIFACTS=$(echo "${BOOTABLE_ARTIFACTS}" | sed -e 's,##SIGNED##,,g')
	for bin in ${BOOT_ARTIFACTS}; do
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
		mv ${DEPLOYDIR}/imx-boot-${MACHINE}-${UBOOT_CONFIG}.bin-${target} ${DEPLOYDIR}/imx-boot-${MACHINE}.bin-${target}
	done
	ln -sf imx-boot-${MACHINE}.bin-${IMAGE_IMXBOOT_TARGET} ${DEPLOYDIR}/imx-boot-${MACHINE}.bin
	ln -sf imx-boot-${MACHINE}.bin-${IMAGE_IMXBOOT_TARGET} ${DEPLOYDIR}/imx-boot
}

deploy_mx95:append:ccimx95() {
    install -m 0644 ${DEPLOY_DIR_IMAGE}/mcore-demos/${CORTEXM_DEFAULT_IMAGE} ${DEPLOYDIR}/${BOOT_TOOLS}
}

do_deploy:append:ccimx8m() {
	generate_symlinks
	for target in ${IMXBOOT_TARGETS}; do
		install -m 0644 ${BOOT_STAGING}/mkimage-${target}.log ${DEPLOYDIR}/${BOOT_TOOLS}
	done
	install -m 0644 ${BOOT_STAGING}/mkimage-print_fit_hab.log ${DEPLOYDIR}/${BOOT_TOOLS}
}

do_deploy:append:ccimx91() {
	generate_symlinks
	for target in ${IMXBOOT_TARGETS}; do
		install -m 0644 ${BOOT_STAGING}/mkimage-${target}.log ${DEPLOYDIR}/${BOOT_TOOLS}
	done
}

do_deploy:append:ccimx93() {
	generate_symlinks
	for target in ${IMXBOOT_TARGETS}; do
		install -m 0644 ${BOOT_STAGING}/mkimage-${target}.log ${DEPLOYDIR}/${BOOT_TOOLS}
		# Generate symlink for SOC revision A0
		if [ "$target" = "flash_singleboot_a0" ]; then
			ln -sf imx-boot-${MACHINE}.bin-${target} ${DEPLOYDIR}/imx-boot-${MACHINE}-A0.bin
		fi
	done
	# Deploy A0 optee binary
	if ${DEPLOY_OPTEE}; then
		# Filename must match the deployed one in "optee-os" recipe for A0 SOC revision
		install -m 0644 ${DEPLOY_DIR_IMAGE}/tee.ccimx93dvk_a0.bin ${DEPLOYDIR}/${BOOT_TOOLS}
	fi
}

do_deploy:append:ccimx95() {
    generate_symlinks
    for target in ${IMXBOOT_TARGETS}; do
        install -m 0644 ${BOOT_STAGING}/mkimage-${target}.log ${DEPLOYDIR}/${BOOT_TOOLS}
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
		cd -
	done

    # Generate an imx-boot symlink to the last SOC_REVISION. This is required for WIC images
    ln -sf ${UBOOT_PREFIX}-${MACHINE}-${rev}.bin-${IMAGE_IMXBOOT_TARGET} ${DEPLOYDIR}/imx-boot
}

do_deploy[postfuncs] += "${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'trustfence_sign_imxboot', '', d)}"
trustfence_sign_imxboot() {
	TF_SIGN_ENV="CONFIG_SIGN_KEYS_PATH=${TRUSTFENCE_KEYS_PATH}"
	TF_SIGN_ENV="$TF_SIGN_ENV CONFIG_FIT_HAB_LOG_PATH=${DEPLOYDIR}/${BOOT_TOOLS}/mkimage-print_fit_hab.log"
	[ -n "${TRUSTFENCE_KEY_INDEX}" ] && TF_SIGN_ENV="$TF_SIGN_ENV CONFIG_KEY_INDEX=${TRUSTFENCE_KEY_INDEX}"
	[ -n "${TRUSTFENCE_SIGN_MODE}" ] && TF_SIGN_ENV="$TF_SIGN_ENV CONFIG_SIGN_MODE=${TRUSTFENCE_SIGN_MODE}"
	[ -n "${TRUSTFENCE_SRK_REVOKE_MASK}" ] && TF_SIGN_ENV="$TF_SIGN_ENV SRK_REVOKE_MASK=${TRUSTFENCE_SRK_REVOKE_MASK}"
	[ -n "${TRUSTFENCE_UNLOCK_KEY_REVOCATION}" ] && TF_SIGN_ENV="$TF_SIGN_ENV CONFIG_UNLOCK_SRK_REVOKE=${TRUSTFENCE_UNLOCK_KEY_REVOCATION}"

	# Sign/encrypt boot image
	for target in ${IMXBOOT_TARGETS}; do
		# Use first "target" as IMAGE_IMXBOOT_TARGET
		if [ "$IMAGE_IMXBOOT_TARGET" = "" ]; then
			IMAGE_IMXBOOT_TARGET="$target"
			echo "Set boot target as $IMAGE_IMXBOOT_TARGET"
		fi
		TF_SIGN_ENV="$TF_SIGN_ENV CONFIG_MKIMAGE_LOG_PATH=${DEPLOYDIR}/${BOOT_TOOLS}/mkimage-${target}.log"
		env $TF_SIGN_ENV trustfence-sign-uboot.sh imx-boot-${MACHINE}.bin-${target} imx-boot-signed-${MACHINE}.bin-${target}
		if [ "${TRUSTFENCE_ENCRYPT}" = "1" ]; then
			TF_ENC_ENV="CONFIG_DEK_PATH=${TRUSTFENCE_KEYS_PATH}/${TRUSTFENCE_DEK_ENCRYPT_KEYNAME} ENABLE_ENCRYPTION=y"
			env $TF_SIGN_ENV $TF_ENC_ENV trustfence-sign-uboot.sh imx-boot-${MACHINE}.bin-${target} imx-boot-encrypted-${MACHINE}.bin-${target}
		fi
	done

	# Generate symlinks for trustfence artifacts.
	ln -sf imx-boot-signed-${MACHINE}.bin-${IMAGE_IMXBOOT_TARGET} ${DEPLOYDIR}/imx-boot-signed-${MACHINE}.bin
	if [ "${TRUSTFENCE_ENCRYPT}" = "1" ]; then
		ln -sf imx-boot-encrypted-${MACHINE}.bin-${IMAGE_IMXBOOT_TARGET} ${DEPLOYDIR}/imx-boot-encrypted-${MACHINE}.bin
	fi
}

trustfence_sign_imxboot:ccimx8x() {
	TF_SIGN_ENV="CONFIG_SIGN_KEYS_PATH=${TRUSTFENCE_KEYS_PATH}"
	[ -n "${TRUSTFENCE_KEY_INDEX}" ] && TF_SIGN_ENV="$TF_SIGN_ENV CONFIG_KEY_INDEX=${TRUSTFENCE_KEY_INDEX}"
	[ -n "${TRUSTFENCE_SIGN_MODE}" ] && TF_SIGN_ENV="$TF_SIGN_ENV CONFIG_SIGN_MODE=${TRUSTFENCE_SIGN_MODE}"
	[ -n "${TRUSTFENCE_SRK_REVOKE_MASK}" ] && TF_SIGN_ENV="$TF_SIGN_ENV SRK_REVOKE_MASK=${TRUSTFENCE_SRK_REVOKE_MASK}"

	# Sign/encrypt boot image
	for target in ${IMXBOOT_TARGETS}; do
		# Use first "target" as IMAGE_IMXBOOT_TARGET
		if [ "$IMAGE_IMXBOOT_TARGET" = "" ]; then
			IMAGE_IMXBOOT_TARGET="$target"
			echo "Set boot target as $IMAGE_IMXBOOT_TARGET"
		fi
		for rev in ${SOC_REVISIONS}; do
			TF_SIGN_ENV="$TF_SIGN_ENV CONFIG_MKIMAGE_LOG_PATH=${DEPLOYDIR}/${BOOT_TOOLS}/mkimage-${rev}-${target}.log"
			env $TF_SIGN_ENV trustfence-sign-uboot.sh imx-boot-${MACHINE}-${rev}.bin-${target} imx-boot-signed-${MACHINE}-${rev}.bin-${target}
			if [ "${TRUSTFENCE_ENCRYPT}" = "1" ]; then
				TF_ENC_ENV="CONFIG_DEK_PATH=${TRUSTFENCE_KEYS_PATH}/${TRUSTFENCE_DEK_ENCRYPT_KEYNAME} ENABLE_ENCRYPTION=y"
				env $TF_SIGN_ENV $TF_ENC_ENV trustfence-sign-uboot.sh imx-boot-${MACHINE}-${rev}.bin-${target} imx-boot-encrypted-${MACHINE}-${rev}.bin-${target}
			fi
		done
	done

	# Generate symlinks for trustfence artifacts.
	for rev in ${SOC_REVISIONS}; do
		ln -sf ${UBOOT_PREFIX}-signed-${MACHINE}-${rev}.bin-${IMAGE_IMXBOOT_TARGET} ${DEPLOYDIR}/${UBOOT_PREFIX}-signed-${MACHINE}-${rev}.bin
		if [ "${TRUSTFENCE_ENCRYPT}" = "1" ]; then
			ln -sf ${UBOOT_PREFIX}-encrypted-${MACHINE}-${rev}.bin-${IMAGE_IMXBOOT_TARGET} ${DEPLOYDIR}/${UBOOT_PREFIX}-encrypted-${MACHINE}-${rev}.bin
		fi
	done
}

trustfence_sign_imxboot[dirs] = "${DEPLOYDIR}"
trustfence_sign_imxboot[vardeps] += "TRUSTFENCE_KEYS_PATH TRUSTFENCE_KEY_INDEX TRUSTFENCE_ENCRYPT TRUSTFENCE_SIGN_MODE TRUSTFENCE_SRK_REVOKE_MASK TRUSTFENCE_UNLOCK_KEY_REVOCATION"
