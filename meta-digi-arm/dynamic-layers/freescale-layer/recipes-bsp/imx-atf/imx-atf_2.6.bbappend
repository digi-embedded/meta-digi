# Copyright (C) 2022 Digi International

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:append = " \
    file://0001-imx8mm-Define-UART1-as-console-for-boot-stage.patch \
    file://0002-imx8mm-Disable-M4-debug-console.patch \
    file://0003-imx8mn-Define-UART1-as-console-for-boot-stage.patch \
    file://0004-imx8mn-Disable-M7-debug-console.patch \
    file://0005-ccimx93-use-UART6-for-the-default-console.patch \
"

# Release "lf-5.15.71-2.2.0"
SRCREV = "3c1583ba0a5d11e5116332e91065cb3740153a46"

BOOT_TOOLS = "imx-boot-tools"

do_deploy:append() {
	install -Dm 0644 ${S}/build/${ATF_PLATFORM}/release/bl31.bin ${DEPLOYDIR}/${BOOT_TOOLS}/bl31-${ATF_PLATFORM}.bin
	if ${BUILD_OPTEE}; then
		install -m 0644 ${S}/build-optee/${ATF_PLATFORM}/release/bl31.bin ${DEPLOYDIR}/${BOOT_TOOLS}/bl31-${ATF_PLATFORM}.bin-optee
	fi
}

COMPATIBLE_MACHINE = "(mx8-generic-bsp|mx9-generic-bsp)"
