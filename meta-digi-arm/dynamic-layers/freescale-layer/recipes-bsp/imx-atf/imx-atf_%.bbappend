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
    file://0006-imx93-bring-back-ELE-clock-workaround-for-soc-revisi.patch \
"

BOOT_TOOLS = "imx-boot-tools"

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
