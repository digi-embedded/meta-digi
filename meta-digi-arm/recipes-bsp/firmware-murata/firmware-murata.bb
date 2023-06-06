# Copyright (C) 2022,2023 Digi International.

SUMMARY = "Murata firmware binaries"
SECTION = "base"
LICENSE = "CYPRESS-EULA"
LIC_FILES_CHKSUM = "file://${S}/cyw-bt-patch/LICENCE.cypress;md5=cbc5f665d04f741f1e006d2096236ba7"

SRC_URI = " \
    git://github.com/murata-wireless/cyw-fmac-fw;protocol=http;branch=fafnir;destsuffix=cyw-fmac-fw;name=cyw-fmac-fw \
    git://github.com/murata-wireless/cyw-fmac-nvram;protocol=http;branch=fafnir;destsuffix=cyw-fmac-nvram;name=cyw-fmac-nvram \
    git://github.com/murata-wireless/cyw-bt-patch;protocol=http;branch=kirkstone-fafnir;destsuffix=cyw-bt-patch;name=cyw-bt-patch \
    git://github.com/murata-wireless/cyw-fmac-utils-imx32;protocol=http;branch=fafnir;destsuffix=cyw-fmac-utils-imx32;name=cyw-fmac-utils-imx32 \
    git://github.com/murata-wireless/cyw-fmac-utils-imx64;protocol=http;branch=fafnir;destsuffix=cyw-fmac-utils-imx64;name=cyw-fmac-utils-imx64 \
    file://cyfmac4373-sdio_US.clm_blob \
"

SRCREV_cyw-fmac-fw="1f83e807b8187508c811a7d91a353a768fef2d37"
SRCREV_cyw-fmac-nvram="303acf1b04441f62fbc48d278a70a099fabacb4a"
SRCREV_cyw-bt-patch="32af98f8bc8a8e123b4c428af0035d74e85ae4bb"
SRCREV_cyw-fmac-utils-imx32="e248804b6ba386fedcd462ddd9394f42f73a17af"
SRCREV_cyw-fmac-utils-imx64="1bc78d68f9609290b2f6578516011c57691f7815"

SRCREV_default = "${AUTOREV}"

S = "${WORKDIR}"

DEPENDS = "libnl"

do_install () {
	bbnote "Installing Murata firmware binaries: "
	install -d ${D}${base_libdir}/firmware/cypress
	install -d ${D}${base_libdir}/firmware/brcm
	install -d ${D}${sbindir}

	# Install Bluetooth patch *.HCD file
	# For Murata 2AE (LBEE5PK2AE-564)
	install -m 444 ${S}/cyw-bt-patch/BCM4373A0.2AE.hcd ${D}${base_libdir}/firmware/brcm/BCM4373A0.hcd

	# Install WLAN firmware file (*.bin) and Regulatory binary file (*.clm_blob)
	# For Murata 2AE (LBEE5PK2AE-564)
	install -m 444 ${S}/cyw-fmac-fw/cyfmac4373-sdio.2AE.bin ${D}${base_libdir}/firmware/cypress/cyfmac4373-sdio.bin
	install -m 444 cyfmac4373-sdio_US.clm_blob ${D}${base_libdir}/firmware/cypress/cyfmac4373-sdio_US.clm_blob

	# Install NVRAM files (*.txt)
	# For Murata 2AE (LBEE5PK2AE-564)
	install -m 444 ${S}/cyw-fmac-nvram/cyfmac4373-sdio.2AE.txt ${D}${base_libdir}/firmware/cypress/cyfmac4373-sdio.txt

	# Install WLAN client utility binary based on 32-bit/64-bit arch
	if [ ${TARGET_ARCH} = "aarch64" ]; then
		install -m 755 ${S}/cyw-fmac-utils-imx64/wl ${D}${sbindir}
	else
		install -m 755 ${S}/cyw-fmac-utils-imx32/wl ${D}${sbindir}
	fi
}

PACKAGES =+ "${PN}-mfgtest"

FILES:${PN} = " \
    ${base_libdir}/firmware \
"

FILES:${PN}-mfgtest = " \
    ${sbindir} \
"

INSANE_SKIP:${PN} += "build-deps"
INSANE_SKIP:${PN} += "file-rdeps"

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccmp1)"
