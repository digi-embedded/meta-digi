# Copyright (C) 2022,2023 Digi International

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:append:ccimx8m = " \
    file://0001-imx8mm-Define-UART1-as-console-for-boot-stage.patch \
    file://0002-imx8mm-Disable-M4-debug-console.patch \
    file://0003-imx8mn-Define-UART1-as-console-for-boot-stage.patch \
    file://0004-imx8mn-Disable-M7-debug-console.patch \
"
SRC_URI:append:ccimx93 = " \
    file://0005-ccimx93-use-UART6-for-the-default-console.patch \
"

BOOT_TOOLS = "imx-boot-tools"

do_deploy:append() {
	install -Dm 0644 ${S}/build/${ATF_PLATFORM}/release/bl31.bin ${DEPLOYDIR}/${BOOT_TOOLS}/bl31-${ATF_PLATFORM}.bin
	if ${BUILD_OPTEE}; then
		install -m 0644 ${S}/build-optee/${ATF_PLATFORM}/release/bl31.bin ${DEPLOYDIR}/${BOOT_TOOLS}/bl31-${ATF_PLATFORM}.bin-optee
	fi
}
