# Copyright 2019,2020 Digi International, Inc.
inherit boot-artifacts

SRC_URI_append_ccimx8mn = " file://0001-imx8m-soc.mak-preserve-dtbs-after-build.patch"

IMX_EXTRA_FIRMWARE_ccimx8x = "digi-sc-firmware imx-seco"

DEPENDS_append_ccimx8x = " coreutils-native"
DEPENDS_append_mx8 += "${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'trustfence-sign-tools-native', '', d)}"

IMX_M7_DEMOS        = ""
IMX_M7_DEMOS_mx8mn  = "imx-m7-demos:do_deploy"

M7_DEFAULT_IMAGE ?= "m7_image.bin"
M7_DEFAULT_IMAGE_mx8mn = "imx8mn_m7_TCM_hello_world.bin"

do_compile[depends] += " \
    ${IMX_M7_DEMOS} \
"

# This package aggregates dependencies with other packages,
# so also define the license dependencies.
do_populate_lic[depends] += " \
	virtual/bootloader:do_populate_lic \
	${@' '.join('%s:do_populate_lic' % r for r in '${IMX_EXTRA_FIRMWARE}'.split() )} \
	imx-atf:do_populate_lic \
	${@bb.utils.contains('IMX_M4_DEMOS', 'imx-m4-demos:do_deploy', 'imx-m4-demos:do_populate_lic', '', d)} \
	${@bb.utils.contains('IMX_M7_DEMOS', 'imx-m7-demos:do_deploy', 'imx-m7-demos:do_populate_lic', '', d)} \
	firmware-imx:do_populate_lic \
"

IMXBOOT_TARGETS_ccimx8x = "${@bb.utils.contains('UBOOT_CONFIG', 'fspi', 'flash_flexspi', \
                                                'flash flash_regression_linux_m4', d)}"

IMXBOOT_TARGETS_ccimx8mn = "${@bb.utils.contains('UBOOT_CONFIG', 'fspi', 'flash_evk_flexspi', 'flash_spl_uboot', d)}"

compile_mx8x() {
	bbnote 8QX boot binary build
	cp ${DEPLOY_DIR_IMAGE}/${M4_DEFAULT_IMAGE}               ${BOOT_STAGING}/m4_image.bin
	cp ${DEPLOY_DIR_IMAGE}/${SECO_FIRMWARE_NAME}             ${BOOT_STAGING}
	cp ${DEPLOY_DIR_IMAGE}/${BOOT_TOOLS}/${ATF_MACHINE_NAME} ${BOOT_STAGING}/bl31.bin
	for type in ${UBOOT_CONFIG}; do
		cp ${DEPLOY_DIR_IMAGE}/${BOOT_TOOLS}/u-boot-${type}.bin           ${BOOT_STAGING}/
	done
	for ramc in ${RAM_CONFIGS}; do
		cp ${DEPLOY_DIR_IMAGE}/${BOOT_TOOLS}/${SC_FIRMWARE_NAME}-${ramc} ${BOOT_STAGING}/
	done
}

compile_mx8m() {
	bbnote 8MQ/8MM/8MN boot binary build
	if [ -e ${DEPLOY_DIR_IMAGE}/${M7_DEFAULT_IMAGE} ] ; then
		cp ${DEPLOY_DIR_IMAGE}/${M7_DEFAULT_IMAGE}           ${BOOT_STAGING}/m7_image.bin
	fi

	for ddr_firmware in ${DDR_FIRMWARE_NAME}; do
		bbnote "Copy ddr_firmware: ${ddr_firmware} from ${DEPLOY_DIR_IMAGE} -> ${BOOT_STAGING} "
		cp ${DEPLOY_DIR_IMAGE}/${ddr_firmware}               ${BOOT_STAGING}
	done
	cp ${DEPLOY_DIR_IMAGE}/signed_dp_imx8m.bin               ${BOOT_STAGING}
	cp ${DEPLOY_DIR_IMAGE}/signed_hdmi_imx8m.bin             ${BOOT_STAGING}
	cp ${DEPLOY_DIR_IMAGE}/u-boot-spl.bin-${MACHINE}-${UBOOT_CONFIG} \
                                                             ${BOOT_STAGING}/u-boot-spl.bin
	cp ${DEPLOY_DIR_IMAGE}/${BOOT_TOOLS}/${UBOOT_DTB_NAME}   ${BOOT_STAGING}
	cp ${DEPLOY_DIR_IMAGE}/${BOOT_TOOLS}/u-boot-nodtb.bin-${MACHINE}-${UBOOT_CONFIG} \
                                                             ${BOOT_STAGING}/u-boot-nodtb.bin
	bbnote "\
Using standard mkimage from u-boot-tools for FIT image builds. The standard \
mkimage is compatible for this use, and using it saves us from having to \
maintain a custom recipe."
	ln -sf ${STAGING_DIR_NATIVE}${bindir}/mkimage            ${BOOT_STAGING}/mkimage_uboot
	cp ${DEPLOY_DIR_IMAGE}/${BOOT_TOOLS}/${ATF_MACHINE_NAME} ${BOOT_STAGING}/bl31.bin
}

do_compile () {
	compile_${SOC_FAMILY}
	if ${DEPLOY_OPTEE}; then
		cp ${DEPLOY_DIR_IMAGE}/tee.bin                       ${BOOT_STAGING}
	fi
	# mkimage for i.MX8
	for type in ${UBOOT_CONFIG}; do
		if [ "${SOC_TARGET}" = "iMX8QX" ]; then
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
							bbnote "building ${SOC_TARGET} - ${ramc} - REV=${rev} ${target}"
							make SOC=${SOC_TARGET} dtbs=${UBOOT_DTB_NAME} REV=${rev} ${target} > mkimage-${target}.log 2>&1
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
		else
			# mkimage for i.MX8M
			for target in ${IMXBOOT_TARGETS}; do
				bbnote "building ${SOC_TARGET} - ${REV_OPTION} ${target}"
				make SOC=${SOC_TARGET} dtbs=${UBOOT_DTB_NAME} ${REV_OPTION} ${target} > mkimage-${target}.log 2>&1
				if [ -e "${BOOT_STAGING}/flash.bin" ]; then
					cp ${BOOT_STAGING}/flash.bin ${S}/${UBOOT_PREFIX}-${MACHINE}.bin-${target}
				fi
			done

			if [ "${TRUSTFENCE_SIGN}" = "1" ]; then
				# Log HAB FIT information
				bbnote "building ${SOC_TARGET} - print_fit_hab"
				make SOC=${SOC_TARGET} dtbs=${UBOOT_DTB_NAME} print_fit_hab > mkimage-print_fit_hab.log 2>&1
			fi
		fi
	done

	# Check that SCFW was built at least once
	if [ "${SOC_TARGET}" = "iMX8QX" and "${SCFWBUILT}" != "yes" ]; then
		bbfatal "SCFW was not built!"
	fi
}

# ConnectCore 8M Nano does not have different binaries
UBOOT_RAM_COMBINATIONS_ccimx8mn = ""

do_install () {
	install -d ${D}/boot
	if [ "${UBOOT_RAM_COMBINATIONS}" = "" ]; then
		for target in ${IMXBOOT_TARGETS}; do
			install -m 0644 ${S}/${UBOOT_PREFIX}-${MACHINE}.bin-${target} ${D}/boot/
		done
	else
		for ramc in ${UBOOT_RAM_COMBINATIONS}; do
			for target in ${IMXBOOT_TARGETS}; do
				for rev in ${SOC_REVISIONS}; do
					install -m 0644 ${S}/${UBOOT_PREFIX}-${MACHINE}-${rev}-${ramc}.bin-${target} ${D}/boot/
				done
			done
		done
	fi
}

deploy_mx8m_append() {
	if [ -e ${BOOT_STAGING}/m7_image.bin ] ; then
		cp ${BOOT_STAGING}/m7_image.bin                      ${DEPLOYDIR}/${BOOT_TOOLS}
	fi
}

do_deploy () {
	deploy_${SOC_FAMILY}
	# copy tee.bin to deploy path
	if "${DEPLOY_OPTEE}"; then
		install -m 0644 ${DEPLOY_DIR_IMAGE}/tee.bin          ${DEPLOYDIR}/${BOOT_TOOLS}
	fi
	# copy makefile (soc.mak) for reference
	install -m 0644 ${BOOT_STAGING}/soc.mak     ${DEPLOYDIR}/${BOOT_TOOLS}
	# copy the generated boot image to deploy path
	if [ "${UBOOT_RAM_COMBINATIONS}" = "" ]; then
		IMAGE_IMXBOOT_TARGET=""
		for target in ${IMXBOOT_TARGETS}; do
			# Use first "target" as IMAGE_IMXBOOT_TARGET
			if [ "$IMAGE_IMXBOOT_TARGET" = "" ]; then
				IMAGE_IMXBOOT_TARGET="$target"
				echo "Set boot target as $IMAGE_IMXBOOT_TARGET"
			fi
			install -m 0644 ${S}/${UBOOT_PREFIX}-${MACHINE}.bin-${target} ${DEPLOYDIR}
		done
		cd ${DEPLOYDIR}
		ln -sf ${UBOOT_PREFIX}-${MACHINE}.bin-${IMAGE_IMXBOOT_TARGET} ${UBOOT_PREFIX}-${MACHINE}.bin
		# Link to default bootable U-Boot filename.
		ln -sf ${UBOOT_PREFIX}-${MACHINE}.bin-${IMAGE_IMXBOOT_TARGET} ${BOOTABLE_FILENAME}
		cd -
	else
		for ramc in ${UBOOT_RAM_COMBINATIONS}; do
			for rev in ${SOC_REVISIONS}; do
				IMAGE_IMXBOOT_TARGET=""
				for target in ${IMXBOOT_TARGETS}; do
					# Use first "target" as IMAGE_IMXBOOT_TARGET
					if [ "$IMAGE_IMXBOOT_TARGET" = "" ]; then
						IMAGE_IMXBOOT_TARGET="$target"
						echo "Set boot target as $IMAGE_IMXBOOT_TARGET"
					fi
					install -m 0644 ${S}/${UBOOT_PREFIX}-${MACHINE}-${rev}-${ramc}.bin-${target} ${DEPLOYDIR}
				done
				cd ${DEPLOYDIR}
				ln -sf ${UBOOT_PREFIX}-${MACHINE}-${rev}-${ramc}.bin-${IMAGE_IMXBOOT_TARGET} ${UBOOT_PREFIX}-${MACHINE}-${rev}-${ramc}.bin
				# Link to default bootable U-Boot filename. It gets overwritten
				# on every loop so the only last RAM_CONFIG will survive.
				ln -sf ${UBOOT_PREFIX}-${MACHINE}-${rev}-${ramc}.bin-${IMAGE_IMXBOOT_TARGET} ${BOOTABLE_FILENAME}
				cd -
			done
		done
	fi

}

do_deploy_append () {
	if [ "${TRUSTFENCE_SIGN}" = "1" ]; then
		export CONFIG_SIGN_KEYS_PATH="${TRUSTFENCE_SIGN_KEYS_PATH}"
		[ -n "${TRUSTFENCE_KEY_INDEX}" ] && export CONFIG_KEY_INDEX="${TRUSTFENCE_KEY_INDEX}"
		[ -n "${TRUSTFENCE_DEK_PATH}" ] && [ "${TRUSTFENCE_DEK_PATH}" != "0" ] && export CONFIG_DEK_PATH="${TRUSTFENCE_DEK_PATH}"
		[ -n "${TRUSTFENCE_SIGN_MODE}" ] && export CONFIG_SIGN_MODE="${TRUSTFENCE_SIGN_MODE}"

		# Sign U-boot image
		if [ "${UBOOT_RAM_COMBINATIONS}" = "" ]; then
			for target in ${IMXBOOT_TARGETS}; do
				# Link to current "target" mkimage log
				ln -sf mkimage-${target}.log mkimage.log
				trustfence-sign-uboot.sh ${DEPLOYDIR}/${UBOOT_PREFIX}-${MACHINE}.bin-${target} ${DEPLOYDIR}/${UBOOT_PREFIX}-signed-${MACHINE}.bin-${target}
			done
		else
			for ramc in ${UBOOT_RAM_COMBINATIONS}; do
				for rev in ${SOC_REVISIONS}; do
					for target in ${IMXBOOT_TARGETS}; do
						# Link to current "target" mkimage log
						ln -sf mkimage-${target}.log mkimage.log
						trustfence-sign-uboot.sh ${DEPLOYDIR}/${UBOOT_PREFIX}-${MACHINE}-${rev}-${ramc}.bin-${target} ${DEPLOYDIR}/${UBOOT_PREFIX}-signed-${MACHINE}-${rev}-${ramc}.bin-${target}

						if [ "${TRUSTFENCE_DEK_PATH}" != "0" ]; then
							export ENABLE_ENCRYPTION=y
							trustfence-sign-uboot.sh ${DEPLOYDIR}/${UBOOT_PREFIX}-${MACHINE}-${rev}-${ramc}.bin-${target} ${DEPLOYDIR}/${UBOOT_PREFIX}-encrypted-${MACHINE}-${rev}-${ramc}.bin-${target}
							unset ENABLE_ENCRYPTION
						fi
					done
				done
			done
		fi

		cp ${B}/SRK_efuses.bin ${DEPLOYDIR}
	fi
}

COMPATIBLE_MACHINE = "(ccimx8x|ccimx8mn)"
