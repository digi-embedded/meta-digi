# Copyright (C) 2022 Digi International

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append = " \
    file://0001-imx8m-soc.mak-preserve-dtbs-after-build.patch \
    file://0002-imx8m-soc.mak-capture-commands-output-into-a-log-fil.patch \
"

# Use NXP's lf-6.1.1_1.0.0 release for ccimx93
SRCBRANCH:ccimx93 = "lf-6.1.1_1.0.0"
SRCREV:ccimx93 = "d489494622585a47b4be88988595b0e4f9598f39"

DEPENDS += "${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'trustfence-sign-tools-native', '', d)}"

# Do not tag imx-boot
UUU_BOOTLOADER = ""
UUU_BOOTLOADER_TAGGED = ""

compile_mx8x() {
    bbnote 8QX boot binary build
    cp ${DEPLOY_DIR_IMAGE}/${SECO_FIRMWARE_NAME}             ${BOOT_STAGING}
    cp ${DEPLOY_DIR_IMAGE}/${ATF_MACHINE_NAME}               ${BOOT_STAGING}/bl31.bin
    for ramc in ${RAM_CONFIGS}; do
        cp ${DEPLOY_DIR_IMAGE}/${BOOT_TOOLS}/${SC_FIRMWARE_NAME}-${ramc} ${BOOT_STAGING}/
    done
    for type in ${UBOOT_CONFIG}; do
        cp ${DEPLOY_DIR_IMAGE}/${BOOT_TOOLS}/u-boot-${type}.bin          ${BOOT_STAGING}/
    done
}

compile_mx8m:append:ccimx8m() {
	# Create dummy DEK blob to support building with encrypted u-boot
	if [ -n "${TRUSTFENCE_DEK_PATH}" ] && [ "${TRUSTFENCE_DEK_PATH}" != "0" ]; then
		dd if=/dev/zero of=${BOOT_STAGING}/dek_blob_fit_dummy.bin bs=96 count=1 oflag=sync
	fi
}

do_compile:append:ccimx8m() {
	bbnote "building ${IMX_BOOT_SOC_TARGET} - print_fit_hab"
	make SOC=${IMX_BOOT_SOC_TARGET} dtbs=${UBOOT_DTB_NAME} print_fit_hab
}

do_compile:ccimx8x () {
	compile_${SOC_FAMILY}
	if ${DEPLOY_OPTEE}; then
		cp ${DEPLOY_DIR_IMAGE}/tee.bin {BOOT_STAGING}
	fi
	# mkimage for i.MX8
	for type in ${UBOOT_CONFIG}; do
		RAM_SIZE="$(echo ${type} | sed -e 's,.*[a-z]\+\([0-9]\+[M|G]B\)$,\1,g')"
		for ramc in ${RAM_CONFIGS}; do
			if echo "${ramc}" | grep -qs "${RAM_SIZE}"; then
				# Match U-Boot memory size and and SCFW memory configuration
				cd ${BOOT_STAGING}
				ln -sf u-boot-${type}.bin u-boot.bin
				ln -sf ${SC_FIRMWARE_NAME}-${ramc} scfw_tcm.bin
				cd -
				for target in ${IMXBOOT_TARGETS}; do
					for rev in ${SOC_REVISIONS}; do
						bbnote "building ${IMX_BOOT_SOC_TARGET} - ${ramc} - REV=${rev} ${target}"
						make SOC=${IMX_BOOT_SOC_TARGET} dtbs=${UBOOT_DTB_NAME} REV=${rev} ${target} > ${S}/mkimage-${target}.log 2>&1
						if [ -e "${BOOT_STAGING}/flash.bin" ]; then
							cp ${BOOT_STAGING}/flash.bin ${S}/${UBOOT_PREFIX}-${MACHINE}-${rev}-${ramc}.bin-${target}
						fi
						SCFWBUILT="yes"
					done
				done
				rm ${BOOT_STAGING}/scfw_tcm.bin
				rm ${BOOT_STAGING}/u-boot.bin
				# Remove u-boot-atf.bin and u-boot-hash.bin so they get generated with the next iteration's U-Boot
				rm ${BOOT_STAGING}/u-boot-atf.bin
				rm ${BOOT_STAGING}/u-boot-hash.bin
			fi
		done
	done

	# Check that SCFW was built at least once
	if [ "${IMX_BOOT_SOC_TARGET}" = "iMX8QX" and "${SCFWBUILT}" != "yes" ]; then
		bbfatal "SCFW was not built!"
	fi
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
	# imx-boot recipe in meta-freescale assumes only *one* build configuration
	# (otherwise variable BOOT_CONFIG_MACHINE would expand to something incorrect)
	for target in ${IMXBOOT_TARGETS}; do
		mv ${DEPLOYDIR}/${BOOT_CONFIG_MACHINE}-${target} ${DEPLOYDIR}/${BOOT_NAME}-${MACHINE}.bin-${target}
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
}

deploy_mx8x() {
    install -d ${DEPLOYDIR}/${BOOT_TOOLS}
    install -m 0644 ${BOOT_STAGING}/${SECO_FIRMWARE_NAME}    ${DEPLOYDIR}/${BOOT_TOOLS}
    install -m 0755 ${S}/${TOOLS_NAME}                       ${DEPLOYDIR}/${BOOT_TOOLS}
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
	for bin in ${BOOTABLE_ARTIFACTS}; do
		IMAGE_IMXBOOT_TARGET=""
		for target in ${IMXBOOT_TARGETS}; do
			# Use first "target" as IMAGE_IMXBOOT_TARGET
			if [ "$IMAGE_IMXBOOT_TARGET" = "" ]; then
				IMAGE_IMXBOOT_TARGET="$target"
				echo "Set boot target as $IMAGE_IMXBOOT_TARGET"
			fi
			install -m 0644 ${S}/${bin}-${target} ${DEPLOYDIR}
			# copy make log for reference
			install -m 0644 ${S}/mkimage-${target}.log ${DEPLOYDIR}/${BOOT_TOOLS}
		done
		cd ${DEPLOYDIR}
		ln -sf ${bin}-${IMAGE_IMXBOOT_TARGET} ${bin}
		# Link to default bootable U-Boot filename. It gets overwritten
		# on every loop so the only last RAM_CONFIG will survive.
		ln -sf ${bin}-${IMAGE_IMXBOOT_TARGET} ${BOOTABLE_FILENAME}
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
trustfence_sign_imxboot[dirs] = "${DEPLOYDIR}"
trustfence_sign_imxboot[vardeps] += "TRUSTFENCE_SIGN_KEYS_PATH TRUSTFENCE_KEY_INDEX TRUSTFENCE_DEK_PATH TRUSTFENCE_SIGN_MODE TRUSTFENCE_SRK_REVOKE_MASK TRUSTFENCE_UNLOCK_KEY_REVOCATION"
