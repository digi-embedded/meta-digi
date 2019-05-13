# Copyright 2019 Digi International, Inc.

# Use the v4.14 ga BSP branch
SRCBRANCH = "imx_4.14.98_2.0.0_ga"
SRCREV = "dd0234001713623c79be92b60fa88bc07b07f24f"

IMX_EXTRA_FIRMWARE_ccimx8x = "digi-sc-firmware"

DEPENDS_append_ccimx8x = " coreutils-native"

# For i.MX 8, this package aggregates the imx-m4-demos
# output. Note that this aggregation replaces the aggregation
# that would otherwise be done in the image build as controlled
# by IMAGE_BOOTFILES_DEPENDS and IMAGE_BOOTFILES in image_types_fsl.bbclass
IMX_M4_DEMOS = "imx-m4-demos"

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

UBOOT_NAME = "u-boot-${MACHINE}.bin"
BOOT_CONFIG_MACHINE = "${BOOT_NAME}"

IMXBOOT_TARGETS_ccimx8x = "${@bb.utils.contains('UBOOT_CONFIG', 'fspi', 'flash_flexspi', \
                                                'flash flash_regression_linux_m4', d)}"

do_compile () {
	bbnote 8QX boot binary build
	cp ${DEPLOY_DIR_IMAGE}/imx8qx_m4_TCM_srtm_demo.bin       ${BOOT_STAGING}/m40_tcm.bin
	cp ${DEPLOY_DIR_IMAGE}/imx8qx_m4_TCM_srtm_demo.bin       ${BOOT_STAGING}/m4_image.bin
	cp ${DEPLOY_DIR_IMAGE}/mx8qx-ahab-container.img          ${BOOT_STAGING}/
	cp ${DEPLOY_DIR_IMAGE}/${BOOT_TOOLS}/${ATF_MACHINE_NAME} ${BOOT_STAGING}/bl31.bin
	for type in ${UBOOT_CONFIG}; do
		RAM_SIZE="$(echo ${type} | sed -e 's,.*\([0-9]\+GB\),\1,g')"
		cp ${DEPLOY_DIR_IMAGE}/${BOOT_TOOLS}/${UBOOT_NAME}-${type}           ${BOOT_STAGING}/u-boot.bin-${type}
		cp ${DEPLOY_DIR_IMAGE}/${BOOT_TOOLS}/${SC_FIRMWARE_NAME}-${RAM_SIZE} ${BOOT_STAGING}/scfw_tcm.bin-${RAM_SIZE}
	done

	# mkimage for i.MX8
	for type in ${UBOOT_CONFIG}; do
		cd ${BOOT_STAGING}
		ln -sf u-boot.bin-${type} u-boot.bin
		RAM_SIZE="$(echo ${type} | sed -e 's,.*\([0-9]\+GB\),\1,g')"
		ln -sf scfw_tcm.bin-${RAM_SIZE} scfw_tcm.bin
		cd -
		for target in ${IMXBOOT_TARGETS}; do
			bbnote "building ${SOC_TARGET} - ${type} - ${target}"
			make SOC=${SOC_TARGET} ${target}
			if [ -e "${BOOT_STAGING}/flash.bin" ]; then
				cp ${BOOT_STAGING}/flash.bin ${S}/${BOOT_CONFIG_MACHINE}-${type}.bin-${target}
			fi
		done
		rm ${BOOT_STAGING}/scfw_tcm.bin
		rm ${BOOT_STAGING}/u-boot.bin
		# Remove u-boot-atf.bin and u-boot-hash.bin so they get generated with the next iteration's U-Boot
		rm ${BOOT_STAGING}/u-boot-atf.bin
		rm ${BOOT_STAGING}/u-boot-hash.bin
	done
}

do_install () {
	install -d ${D}/boot
	for type in ${UBOOT_CONFIG}; do
		for target in ${IMXBOOT_TARGETS}; do
			install -m 0644 ${S}/${BOOT_CONFIG_MACHINE}-${type}.bin-${target} ${D}/boot/
		done
	done
}

do_deploy () {
	install -d ${DEPLOYDIR}/${BOOT_TOOLS}

	# copy the tool mkimage to deploy path and sc fw, dcd and uboot
	install -m 0644 ${BOOT_STAGING}/mx8qx-ahab-container.img ${DEPLOYDIR}/${BOOT_TOOLS}
	install -m 0644 ${BOOT_STAGING}/m40_tcm.bin              ${DEPLOYDIR}/${BOOT_TOOLS}
	install -m 0644 ${BOOT_STAGING}/m4_image.bin             ${DEPLOYDIR}/${BOOT_TOOLS}
	install -m 0755 ${S}/${TOOLS_NAME}                       ${DEPLOYDIR}/${BOOT_TOOLS}

	# copy makefile (soc.mak) for reference
	install -m 0644 ${BOOT_STAGING}/soc.mak     ${DEPLOYDIR}/${BOOT_TOOLS}

	# copy the generated boot image to deploy path
	for type in ${UBOOT_CONFIG}; do
		IMAGE_IMXBOOT_TARGET=""
		for target in ${IMXBOOT_TARGETS}; do
			# Use first "target" as IMAGE_IMXBOOT_TARGET
			if [ "$IMAGE_IMXBOOT_TARGET" = "" ]; then
				IMAGE_IMXBOOT_TARGET="$target"
				echo "Set boot target as $IMAGE_IMXBOOT_TARGET"
			fi
			install -m 0644 ${S}/${BOOT_CONFIG_MACHINE}-${type}.bin-${target} ${DEPLOYDIR}
		done
	cd ${DEPLOYDIR}
	ln -sf ${BOOT_CONFIG_MACHINE}-${type}.bin-${IMAGE_IMXBOOT_TARGET} ${BOOT_CONFIG_MACHINE}-${type}.bin
	ln -sf ${BOOT_CONFIG_MACHINE}-${type}.bin-${IMAGE_IMXBOOT_TARGET} ${BOOT_CONFIG_MACHINE}-${MACHINE}.bin
	cd -
	done
}

COMPATIBLE_MACHINE = "(ccimx8x)"
