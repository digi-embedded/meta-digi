# Copyright 2017-2021 NXP
# Copyright 2019-2021 Digi International, Inc.

require imx-mkimage_git.inc

IMX_M4_DEMOS      = ""
IMX_M4_DEMOS_mx8  = "imx-m4-demos:do_deploy"
IMX_M4_DEMOS_mx8m = ""

M4_DEFAULT_IMAGE ?= "m4_image.bin"
M4_DEFAULT_IMAGE_mx8qxp = "imx8qx_m4_TCM_power_mode_switch.bin"
M4_DEFAULT_IMAGE_mx8dxl = "imx8dxl_m4_TCM_power_mode_switch.bin"
M4_DEFAULT_IMAGE_mx8dx = "imx8qx_m4_TCM_power_mode_switch.bin"

# Setting for i.MX 8ULP
IMX_M4_DEMOS_mx8ulp = "imx-m33-demos:do_deploy"
M4_DEFAULT_IMAGE_mx8ulp = "imx8ulp_m33_TCM_rpmsg_lite_str_echo_rtos.bin"
ATF_MACHINE_NAME_mx8ulp = "bl31-imx8ulp.bin"
IMX_EXTRA_FIRMWARE_mx8ulp = "firmware-upower firmware-sentinel"
SECO_FIRMWARE_NAME_mx8ulp = "mx8ulpa0-ahab-container.img"
SOC_TARGET_mx8ulp = "iMX8ULP"
SOC_FAMILY_mx8ulp = "mx8ulp"


do_compile[depends] += "${IMX_M4_DEMOS}"

do_compile_prepend() {
    case ${SOC_FAMILY} in
    mx8)
        cp ${DEPLOY_DIR_IMAGE}/imx8qm_m4_TCM_power_mode_switch_m40.bin \
                                                             ${BOOT_STAGING}/m4_image.bin
        cp ${DEPLOY_DIR_IMAGE}/imx8qm_m4_TCM_power_mode_switch_m41.bin \
                                                             ${BOOT_STAGING}/m4_1_image.bin
        ;;
    mx8x)
        cp ${DEPLOY_DIR_IMAGE}/${M4_DEFAULT_IMAGE}           ${BOOT_STAGING}/m4_image.bin
        ;;
    mx8ulp)
        cp ${DEPLOY_DIR_IMAGE}/${M4_DEFAULT_IMAGE}       ${BOOT_STAGING}/m33_image.bin
        ;;
    esac
}

compile_mx8ulp() {
    bbnote 8ULP boot binary build
    cp ${DEPLOY_DIR_IMAGE}/${BOOT_TOOLS}/${ATF_MACHINE_NAME} ${BOOT_STAGING}/bl31.bin
    cp ${DEPLOY_DIR_IMAGE}/${UBOOT_NAME}                     ${BOOT_STAGING}/u-boot.bin
    if [ -e ${DEPLOY_DIR_IMAGE}/u-boot-spl.bin-${MACHINE}-${UBOOT_CONFIG} ] ; then
        cp ${DEPLOY_DIR_IMAGE}/u-boot-spl.bin-${MACHINE}-${UBOOT_CONFIG} \
                                                             ${BOOT_STAGING}/u-boot-spl.bin
    fi

    # Copy SECO F/W and upower.bin
    cp ${DEPLOY_DIR_IMAGE}/${BOOT_TOOLS}/${SECO_FIRMWARE_NAME}  ${BOOT_STAGING}/
    cp ${DEPLOY_DIR_IMAGE}/${BOOT_TOOLS}/upower.bin          ${BOOT_STAGING}/upower.bin
}

do_deploy_append() {
    case ${SOC_FAMILY} in
    mx8)
        install -m 0644 ${BOOT_STAGING}/m4_image.bin         ${DEPLOYDIR}/${BOOT_TOOLS}
        install -m 0644 ${BOOT_STAGING}/m4_1_image.bin       ${DEPLOYDIR}/${BOOT_TOOLS}
        ;;
    mx8x)
        install -m 0644 ${BOOT_STAGING}/m4_image.bin         ${DEPLOYDIR}/${BOOT_TOOLS}
        ;;
    mx8ulp)
        install -m 0644 ${BOOT_STAGING}/m33_image.bin        ${DEPLOYDIR}/${BOOT_TOOLS}
        ;;
    esac

    # Digi: omit this step to avoid build errors
    # Append a tag to the bootloader image used in the SD card image
    #cp ${DEPLOYDIR}/${BOOT_NAME}                             ${DEPLOYDIR}/${BOOT_NAME}-tagged
    #ln -sf ${BOOT_NAME}-tagged                               ${DEPLOYDIR}/${BOOT_NAME}
    #stat -L -cUUUBURNXXOEUZX7+A-XY5601QQWWZ%sEND ${DEPLOYDIR}/${BOOT_NAME} \
    #                                                      >> ${DEPLOYDIR}/${BOOT_NAME}
}

deploy_mx8ulp() {
    install -d ${DEPLOYDIR}/${BOOT_TOOLS}
    install -m 0755 ${S}/${TOOLS_NAME}                       ${DEPLOYDIR}/${BOOT_TOOLS}
    if [ -e ${DEPLOY_DIR_IMAGE}/u-boot-spl.bin-${MACHINE}-${UBOOT_CONFIG} ] ; then
        install -m 0644 ${DEPLOY_DIR_IMAGE}/u-boot-spl.bin-${MACHINE}-${UBOOT_CONFIG} \
                                                             ${DEPLOYDIR}/${BOOT_TOOLS}
    fi
}

#######################
# Digi customizations #
#######################
inherit boot-artifacts

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append_ccimx8m = " file://0001-imx8m-soc.mak-preserve-dtbs-after-build.patch"

IMX_EXTRA_FIRMWARE_ccimx8x = "digi-sc-firmware imx-seco"

IMX_BOOT_SOC_TARGET_mx8mm = "iMX8MM"
IMX_BOOT_SOC_TARGET_mx8mn = "iMX8MN"
IMX_BOOT_SOC_TARGET_mx8x = "iMX8QX"

DEPENDS_append_ccimx8x = " coreutils-native"
DEPENDS_append_mx8 += "${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'trustfence-sign-tools-native', '', d)}"

IMX_M4_DEMOS_mx8mm   = "imx-m4-demos:do_deploy"

M4_DEFAULT_IMAGE_mx8mm = "imx8mm_m4_TCM_hello_world.bin"

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

IMXBOOT_TARGETS_ccimx8m = "${@bb.utils.contains('UBOOT_CONFIG', 'fspi', 'flash_evk_flexspi', 'flash_spl_uboot', d)}"

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
	# Create dummy DEK blob
	if [ "${TRUSTFENCE_DEK_PATH}" != "0" ]; then
		dd if=/dev/zero of=${BOOT_STAGING}/dek_blob_fit_dummy.bin bs=96 count=1 oflag=sync
	fi
}

do_compile () {
	compile_${SOC_FAMILY}
	if ${DEPLOY_OPTEE}; then
		cp ${DEPLOY_DIR_IMAGE}/tee.bin                       ${BOOT_STAGING}
	fi
	# mkimage for i.MX8
	for type in ${UBOOT_CONFIG}; do
		if [ "${IMX_BOOT_SOC_TARGET}" = "iMX8QX" ]; then
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
		else
			# mkimage for i.MX8M
			for target in ${IMXBOOT_TARGETS}; do
				bbnote "building ${IMX_BOOT_SOC_TARGET} - ${REV_OPTION} ${target}"
				make SOC=${IMX_BOOT_SOC_TARGET} dtbs=${UBOOT_DTB_NAME} ${REV_OPTION} ${target} > ${S}/mkimage-${target}.log 2>&1
				if [ -e "${BOOT_STAGING}/flash.bin" ]; then
					cp ${BOOT_STAGING}/flash.bin ${S}/${UBOOT_PREFIX}-${MACHINE}.bin-${target}
				fi
			done

			# Log HAB FIT information
			bbnote "building ${IMX_BOOT_SOC_TARGET} - print_fit_hab"
			make SOC=${IMX_BOOT_SOC_TARGET} dtbs=${UBOOT_DTB_NAME} print_fit_hab > ${S}/mkimage-print_fit_hab.log 2>&1
		fi
	done

	# Check that SCFW was built at least once
	if [ "${IMX_BOOT_SOC_TARGET}" = "iMX8QX" and "${SCFWBUILT}" != "yes" ]; then
		bbfatal "SCFW was not built!"
	fi
}

# ConnectCore 8M Nano and 8M Mini do not have different binaries
UBOOT_RAM_COMBINATIONS_ccimx8m = ""

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
			# copy make log for reference
			install -m 0644 ${S}/mkimage-${target}.log ${DEPLOYDIR}/${BOOT_TOOLS}
		done
		# copy fit_hab log for reference
		install -m 0644 ${S}/mkimage-print_fit_hab.log ${DEPLOYDIR}/${BOOT_TOOLS}
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
					# copy make log for reference
					install -m 0644 ${S}/mkimage-${target}.log ${DEPLOYDIR}/${BOOT_TOOLS}
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
		[ -n "${TRUSTFENCE_SRK_REVOKE_MASK}" ] && export SRK_REVOKE_MASK="${TRUSTFENCE_SRK_REVOKE_MASK}"
		[ -n "${TRUSTFENCE_UNLOCK_KEY_REVOCATION}" ] && export CONFIG_UNLOCK_SRK_REVOKE="${TRUSTFENCE_UNLOCK_KEY_REVOCATION}"

		# Sign U-boot image
		if [ "${UBOOT_RAM_COMBINATIONS}" = "" ]; then
			for target in ${IMXBOOT_TARGETS}; do
				# Point to make logs
				export CONFIG_MKIMAGE_LOG_PATH="${DEPLOYDIR}/${BOOT_TOOLS}/mkimage-${target}.log"
				export CONFIG_FIT_HAB_LOG_PATH="${DEPLOYDIR}/${BOOT_TOOLS}/mkimage-print_fit_hab.log"
				trustfence-sign-uboot.sh ${DEPLOYDIR}/${UBOOT_PREFIX}-${MACHINE}.bin-${target} ${DEPLOYDIR}/${UBOOT_PREFIX}-signed-${MACHINE}.bin-${target}

				if [ "${TRUSTFENCE_DEK_PATH}" != "0" ]; then
					export ENABLE_ENCRYPTION=y
					trustfence-sign-uboot.sh ${DEPLOYDIR}/${UBOOT_PREFIX}-${MACHINE}.bin-${target} ${DEPLOYDIR}/${UBOOT_PREFIX}-encrypted-${MACHINE}.bin-${target}
					unset ENABLE_ENCRYPTION
				fi
			done
		else
			for ramc in ${UBOOT_RAM_COMBINATIONS}; do
				for rev in ${SOC_REVISIONS}; do
					for target in ${IMXBOOT_TARGETS}; do
						# Point to make log
						export CONFIG_MKIMAGE_LOG_PATH="${DEPLOYDIR}/${BOOT_TOOLS}/mkimage-${target}.log"
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

COMPATIBLE_MACHINE = "(ccimx8x|ccimx8m)"
