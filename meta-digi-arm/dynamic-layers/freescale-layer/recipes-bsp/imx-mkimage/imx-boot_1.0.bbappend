# Copyright (C) 2022 Digi International

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append = " \
    file://0001-imx8m-soc.mak-preserve-dtbs-after-build.patch \
"

SOC_FAMILY:mx9-nxp-bsp = "mx93"

# Do not tag imx-boot
UUU_BOOTLOADER = ""
UUU_BOOTLOADER_TAGGED = ""

compile_mx93() {
	bbnote "i.MX 93 boot binary build"
	for ddr_firmware in ${DDR_FIRMWARE_NAME}; do
		bbnote "Copy ddr_firmware: ${ddr_firmware} from ${DEPLOY_DIR_IMAGE} -> ${BOOT_STAGING}"
		cp ${DEPLOY_DIR_IMAGE}/${ddr_firmware} ${BOOT_STAGING}
	done

	cp ${DEPLOY_DIR_IMAGE}/${SECO_FIRMWARE_NAME} ${BOOT_STAGING}/
	cp ${DEPLOY_DIR_IMAGE}/${BOOT_TOOLS}/${ATF_MACHINE_NAME} ${BOOT_STAGING}/bl31.bin
	cp ${DEPLOY_DIR_IMAGE}/${UBOOT_NAME} ${BOOT_STAGING}/u-boot.bin
	if [ -e ${DEPLOY_DIR_IMAGE}/u-boot-spl.bin-${MACHINE}-${UBOOT_CONFIG} ] ; then
		cp ${DEPLOY_DIR_IMAGE}/u-boot-spl.bin-${MACHINE}-${UBOOT_CONFIG} ${BOOT_STAGING}/u-boot-spl.bin
	fi
}

deploy_mx93() {
	install -d ${DEPLOYDIR}/${BOOT_TOOLS}
	for ddr_firmware in ${DDR_FIRMWARE_NAME}; do
		install -m 0644 ${DEPLOY_DIR_IMAGE}/${ddr_firmware} ${DEPLOYDIR}/${BOOT_TOOLS}
	done

	install -m 0644 ${BOOT_STAGING}/${SECO_FIRMWARE_NAME} ${DEPLOYDIR}/${BOOT_TOOLS}
	install -m 0755 ${S}/${TOOLS_NAME} ${DEPLOYDIR}/${BOOT_TOOLS}
	if [ -e ${DEPLOY_DIR_IMAGE}/u-boot-spl.bin-${MACHINE}-${UBOOT_CONFIG} ]; then
		install -m 0644 ${DEPLOY_DIR_IMAGE}/u-boot-spl.bin-${MACHINE}-${UBOOT_CONFIG} ${DEPLOYDIR}/${BOOT_TOOLS}
	fi
}

do_deploy:append() {
	# DEY install scripts use by default INSTALL_UBOOT_FILENAME="imx-boot-##MACHINE##.bin"
	ln -sf ${BOOT_CONFIG_MACHINE}-${IMAGE_IMXBOOT_TARGET} ${DEPLOYDIR}/${BOOT_NAME}-${MACHINE}.bin
}

COMPATIBLE_MACHINE = "(mx8-generic-bsp|mx9-generic-bsp)"
