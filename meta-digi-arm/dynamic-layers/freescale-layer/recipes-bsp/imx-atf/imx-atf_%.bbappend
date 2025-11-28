# Copyright (C) 2022-2025, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:append:dey = " \
    file://0001-imx8mm-Define-UART1-as-console-for-boot-stage.patch \
    file://0002-imx8mm-Disable-M4-debug-console.patch \
    file://0003-imx8mn-Define-UART1-as-console-for-boot-stage.patch \
    file://0004-imx8mn-Disable-M7-debug-console.patch \
    file://0005-imx8mm-set-BL32_BASE-and-map-high-DRAM-for-ccimx8mm-.patch \
    file://0006-ccimx93-use-UART6-for-the-default-console.patch \
    file://0007-imx93-bring-back-ELE-clock-workaround-for-soc-revisi.patch \
    file://0008-ccimx91-use-UART6-for-the-default-console.patch \
    file://0009-ccimx95-set-DVK-console-to-LPUART6.patch \
    file://0010-ccimx95-enable-non-secure-non-privilege-access-to-GP.patch \
"

BOOT_TOOLS = "imx-boot-tools"

EXTRA_OEMAKE += "${@oe.utils.conditional('TRUSTFENCE_CONSOLE_DISABLE', '1', 'LOG_LEVEL=0', '', d)}"

# Build ATF for imx93 SOC revision A0
do_compile:append:ccimx93() {
	oe_runmake SOC_REV_A0=1 BUILD_BASE=build-A0 clean
	oe_runmake SOC_REV_A0=1 BUILD_BASE=build-A0 bl31
	if ${BUILD_OPTEE}; then
		oe_runmake SOC_REV_A0=1 BUILD_BASE=build-A0-optee clean
		oe_runmake SOC_REV_A0=1 BUILD_BASE=build-A0-optee SPD=opteed bl31
	fi
}

do_deploy:append() {
	install -Dm 0644 ${S}/build/${ATF_PLATFORM}/release/bl31.bin ${DEPLOYDIR}/${BOOT_TOOLS}/bl31-${ATF_PLATFORM}.bin
	if ${BUILD_OPTEE}; then
		install -m 0644 ${S}/build-optee/${ATF_PLATFORM}/release/bl31.bin ${DEPLOYDIR}/${BOOT_TOOLS}/bl31-${ATF_PLATFORM}.bin-optee
	fi
}

# Deploy ATF for imx93 SOC revision A0
do_deploy:append:ccimx93() {
	install -Dm 0644 ${S}/build-A0/${ATF_PLATFORM}/release/bl31.bin ${DEPLOYDIR}/bl31-${ATF_PLATFORM}-A0.bin
	install -Dm 0644 ${S}/build-A0/${ATF_PLATFORM}/release/bl31.bin ${DEPLOYDIR}/${BOOT_TOOLS}/bl31-${ATF_PLATFORM}-A0.bin
	if ${BUILD_OPTEE}; then
		install -m 0644 ${S}/build-A0-optee/${ATF_PLATFORM}/release/bl31.bin ${DEPLOYDIR}/bl31-${ATF_PLATFORM}-A0.bin-optee
		install -m 0644 ${S}/build-A0-optee/${ATF_PLATFORM}/release/bl31.bin ${DEPLOYDIR}/${BOOT_TOOLS}/bl31-${ATF_PLATFORM}-A0.bin-optee
	fi
}
