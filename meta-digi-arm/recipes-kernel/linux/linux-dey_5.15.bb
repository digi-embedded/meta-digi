# Copyright (C) 2022,2023 Digi International

require recipes-kernel/linux/linux-dey.inc

SRCBRANCH = "v5.15/nxp/dey-4.0/maint"
SRCBRANCH:stm32mpcommon = "v5.15/stm/dey-4.0/maint"
SRCREV = "${AUTOREV}"
SRCREV:stm32mpcommon = "${AUTOREV}"

do_assemble_fitimage:prepend:ccmp1() {
	# Deploy u-boot script to be included into the FIT image
	install -d ${STAGING_DIR_HOST}/boot
	install -m 0644 ${RECIPE_SYSROOT}/${datadir}/${UBOOT_ENV_BINARY} ${STAGING_DIR_HOST}/boot/
}

COMPATIBLE_MACHINE = "(ccimx6|ccimx6ul|ccimx8m|ccimx8x|ccmp1)"
