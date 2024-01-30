# Copyright (C) 2022,2023 Digi International

require u-boot-dey.inc
LIC_FILES_CHKSUM = "file://Licenses/README;md5=5a7450c57ffe5ae63fd732446b988025"

DEPENDS += "flex-native bison-native"
DEPENDS += "python3-setuptools-native"

SRCBRANCH = "v2021.10/maint"
SRCREV = "b790ac5d6d1425604deefd8885e93f1a016d73b5"

UBOOT_FIT_CFG_FRAGMENTS = " \
    file://fit_legacy.cfg \
    file://fit_signature.cfg \
"

SRC_URI += " \
    ${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', '${UBOOT_FIT_CFG_FRAGMENTS}', '', d)} \
"
# Install UBOOT_ENV_BINARY to datadir, so that kernel can use it
# to include it into the FIT image.
install_helper_bootscr() {
	if [ -f "${D}/boot/${UBOOT_ENV_BINARY}" ]; then
		# Install UBOOT_ENV_BINARY into datadir to share it with the kernel
		install -Dm 0644 ${D}/boot/${UBOOT_ENV_BINARY} ${D}${datadir}/${UBOOT_ENV_IMAGE}
		ln -sf ${UBOOT_ENV_IMAGE} ${D}${datadir}/${UBOOT_ENV_BINARY}
	else
		bbwarn "${D}/boot/${UBOOT_ENV_BINARY} not found"
	fi
}

do_install:append() {
	# Copy boot script, so kernel can include it when creating the FIT image 
	if [ "${TRUSTFENCE_FIT_IMG}" = "1" ] && [ -n "${UBOOT_ENV_BINARY}" ]; then
		install_helper_bootscr
	fi
}

COMPATIBLE_MACHINE = "(ccmp1)"
