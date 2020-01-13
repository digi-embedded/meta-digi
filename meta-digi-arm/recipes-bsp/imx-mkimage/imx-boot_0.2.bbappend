# Copyright 2019,2020 Digi International, Inc.
inherit boot-artifacts

# Use the v4.14 latest BSP branch
SRCBRANCH = "imx_4.14.98_2.2.0"
SRCREV = "c00cd78d2e80178171d2d7f8d0d1ce6e2ea41ac5"

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"
SRC_URI_append_ccimx8x = " file://0001-iMX8QX-remove-SC_BD_FLAGS_ALT_CONFIG-flag-in-flash_r.patch"

IMX_EXTRA_FIRMWARE_ccimx8x = "digi-sc-firmware imx-seco"

DEPENDS_append_ccimx8x = " coreutils-native"

# For i.MX 8, this package aggregates the imx-m4-demos
# output. Note that this aggregation replaces the aggregation
# that would otherwise be done in the image build as controlled
# by IMAGE_BOOTFILES_DEPENDS and IMAGE_BOOTFILES in image_types_fsl.bbclass
IMX_M4_DEMOS        = ""
IMX_M4_DEMOS_mx8qm  = "imx-m4-demos"
IMX_M4_DEMOS_mx8qxp = "imx-m4-demos"

# This package aggregates output deployed by other packages,
# so set the appropriate dependencies
do_compile[depends] += " \
	${@' '.join('%s:do_deploy' % r for r in '${IMX_M4_DEMOS}'.split() )} \
	firmware-imx:do_deploy \
"

# This package aggregates dependencies with other packages,
# so also define the license dependencies.
do_populate_lic[depends] += " \
	virtual/bootloader:do_populate_lic \
	${@' '.join('%s:do_populate_lic' % r for r in '${IMX_EXTRA_FIRMWARE}'.split() )} \
	imx-atf:do_populate_lic \
	${@' '.join('%s:do_populate_lic' % r for r in '${IMX_M4_DEMOS}'.split() )} \
	firmware-imx:do_populate_lic \
"
ATF_MACHINE_NAME_mx8qxp = "bl31-imx8qx.bin"
ATF_MACHINE_NAME_mx8mn = "bl31-imx8mn.bin"

SECO_FIRMWARE ?= ""
SECO_FIRMWARE_mx8qm  = "mx8qmb0-ahab-container.img"
SECO_FIRMWARE_mx8qxp = "mx8qxb0-ahab-container.img"
# i.MX8QXP C0 support
#SECO_FIRMWARE_mx8qxp = "mx8qxc0-ahab-container.img"

# 8MQ/8MM/8MN share the same soc folder
BOOT_STAGING_mx8mn = "${S}/iMX8M"

SOC_TARGET_mx8mn  = "iMX8MN"

IMXBOOT_TARGETS_ccimx8x = "${@bb.utils.contains('UBOOT_CONFIG', 'fspi', 'flash_flexspi', \
                                                'flash flash_regression_linux_m4', d)}"

IMXBOOT_TARGETS_ccimx8mn = "${@bb.utils.contains('UBOOT_CONFIG', 'fspi', 'flash_evk_flexspi', 'flash_spl_uboot', d)}"

compile_mx8x() {
	bbnote 8QX boot binary build
	cp ${DEPLOY_DIR_IMAGE}/imx8qx_m4_TCM_power_mode_switch.bin       ${BOOT_STAGING}/m40_tcm.bin
	cp ${DEPLOY_DIR_IMAGE}/imx8qx_m4_TCM_power_mode_switch.bin       ${BOOT_STAGING}/m4_image.bin
	cp ${DEPLOY_DIR_IMAGE}/${SECO_FIRMWARE}          ${BOOT_STAGING}/
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
	cp ${DEPLOY_DIR_IMAGE}/signed_*_imx8m.bin                ${BOOT_STAGING}
	cp ${DEPLOY_DIR_IMAGE}/${BOOT_TOOLS}/u-boot-spl.bin-${MACHINE}-${UBOOT_CONFIG} ${BOOT_STAGING}/u-boot-spl.bin
	cp ${DEPLOY_DIR_IMAGE}/${BOOT_TOOLS}/${UBOOT_DTB_NAME}   ${BOOT_STAGING}/fsl-imx8mn-evk.dtb
	cp ${DEPLOY_DIR_IMAGE}/${BOOT_TOOLS}/u-boot-nodtb.bin-${MACHINE}-${UBOOT_CONFIG}    ${BOOT_STAGING}/u-boot-nodtb.bin

	cp ${DEPLOY_DIR_IMAGE}/${BOOT_TOOLS}/mkimage_uboot       ${BOOT_STAGING}/
	cp ${DEPLOY_DIR_IMAGE}/${BOOT_TOOLS}/${ATF_MACHINE_NAME} ${BOOT_STAGING}/bl31.bin

	for ddr_firmware in ${DDR_FIRMWARE_NAME}; do
		cp ${DEPLOY_DIR_IMAGE}/${ddr_firmware} ${BOOT_STAGING}
	done
}

do_compile () {
	compile_${SOC_FAMILY}
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
					    bbnote "building ${SOC_TARGET} - ${ramc} - ${target}"
					    make SOC=${SOC_TARGET} ${target}
					    # i.MX8QXP C0 support
					    #make SOC=${SOC_TARGET} REV=C0 ${target}
					    if [ -e "${BOOT_STAGING}/flash.bin" ]; then
						    cp ${BOOT_STAGING}/flash.bin ${S}/${UBOOT_PREFIX}-${MACHINE}-${ramc}.bin-${target}
					    fi
					    SCFWBUILT="yes"
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
			bbnote "building ${SOC_TARGET} - ${target}"
			make SOC=${SOC_TARGET} ${target}
			if [ -e "${BOOT_STAGING}/flash.bin" ]; then
			    cp ${BOOT_STAGING}/flash.bin ${S}/${UBOOT_PREFIX}-${MACHINE}.bin-${target}
			fi
		    done
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
				install -m 0644 ${S}/${UBOOT_PREFIX}-${MACHINE}-${ramc}.bin-${target} ${D}/boot/
			done
		done
	fi
}

do_deploy () {
	install -d ${DEPLOYDIR}/${BOOT_TOOLS}

	# copy the tool mkimage to deploy path and sc fw, dcd and uboot
	if [ "${SOC_TARGET}" = "iMX8QX" ]; then
		install -m 0644 ${BOOT_STAGING}/${SECO_FIRMWARE} ${DEPLOYDIR}/${BOOT_TOOLS}
		install -m 0644 ${BOOT_STAGING}/m40_tcm.bin              ${DEPLOYDIR}/${BOOT_TOOLS}
		install -m 0644 ${BOOT_STAGING}/m4_image.bin             ${DEPLOYDIR}/${BOOT_TOOLS}
	fi
	install -m 0755 ${S}/${TOOLS_NAME}                       ${DEPLOYDIR}/${BOOT_TOOLS}

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
		# Link to default bootable U-Boot filename. It gets overwritten
		# on every loop so the only last RAM_CONFIG will survive.
		ln -sf ${UBOOT_PREFIX}-${MACHINE}.bin-${IMAGE_IMXBOOT_TARGET} ${BOOTABLE_FILENAME}
		cd -
	else
		for ramc in ${UBOOT_RAM_COMBINATIONS}; do
			IMAGE_IMXBOOT_TARGET=""
			for target in ${IMXBOOT_TARGETS}; do
				# Use first "target" as IMAGE_IMXBOOT_TARGET
				if [ "$IMAGE_IMXBOOT_TARGET" = "" ]; then
					IMAGE_IMXBOOT_TARGET="$target"
					echo "Set boot target as $IMAGE_IMXBOOT_TARGET"
				fi
				install -m 0644 ${S}/${UBOOT_PREFIX}-${MACHINE}-${ramc}.bin-${target} ${DEPLOYDIR}
			done
			cd ${DEPLOYDIR}
			ln -sf ${UBOOT_PREFIX}-${MACHINE}-${ramc}.bin-${IMAGE_IMXBOOT_TARGET} ${UBOOT_PREFIX}-${MACHINE}-${ramc}.bin
			# Link to default bootable U-Boot filename. It gets overwritten
			# on every loop so the only last RAM_CONFIG will survive.
			ln -sf ${UBOOT_PREFIX}-${MACHINE}-${ramc}.bin-${IMAGE_IMXBOOT_TARGET} ${BOOTABLE_FILENAME}
			cd -
		done
	fi

}

COMPATIBLE_MACHINE = "(ccimx8x|ccimx8mn)"
