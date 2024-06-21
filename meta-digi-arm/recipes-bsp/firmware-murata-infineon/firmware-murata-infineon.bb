# Copyright (C) 2022-2024 Digi International Inc.

SUMMARY = "Murata Infineon firmware binaries"
SECTION = "base"
LICENSE = "CYPRESS-EULA"
LIC_FILES_CHKSUM = "file://${S}/cyw-bt-patch/LICENCE.cypress;md5=cbc5f665d04f741f1e006d2096236ba7"

SRC_URI = " \
    git://github.com/murata-wireless/cyw-fmac-fw;protocol=http;branch=hedorah;destsuffix=cyw-fmac-fw;name=cyw-fmac-fw \
    git://github.com/murata-wireless/cyw-fmac-nvram;protocol=http;branch=hedorah;destsuffix=cyw-fmac-nvram;name=cyw-fmac-nvram \
    git://github.com/murata-wireless/cyw-bt-patch;protocol=http;branch=mickledore-hedorah;destsuffix=cyw-bt-patch;name=cyw-bt-patch \
    git://github.com/murata-wireless/cyw-fmac-utils-imx32;protocol=http;branch=master;destsuffix=cyw-fmac-utils-imx32;name=cyw-fmac-utils-imx32 \
    git://github.com/murata-wireless/cyw-fmac-utils-imx64;protocol=http;branch=master;destsuffix=cyw-fmac-utils-imx64;name=cyw-fmac-utils-imx64 \
    file://cyfmac4373-sdio_US.clm_blob \
    file://cyfmac4373-sdio_World.clm_blob \
    file://cyw4373-autocountry \
    file://cyw4373-autocountry.service \
"

SRCREV_cyw-fmac-fw="db8deb03b8d24e5069ac4581d1c35b767012e926"
SRCREV_cyw-fmac-nvram="9b7d93eb3e13b2d2ed8ce3a01338ceb54151b77a"
SRCREV_cyw-bt-patch="3275a7036dd0d6eacecccccc760b7e7fe91a9e32"
SRCREV_cyw-fmac-utils-imx32="fcdd231e9bb23db3c93c10e5dff43a1182f220c5"
SRCREV_cyw-fmac-utils-imx64="52cc4cc6be8629781014505aa276b67e18cf6e8d"

SRCREV_default = "${AUTOREV}"

S = "${WORKDIR}"

DEPENDS = "libnl"

do_install () {
	bbnote "Installing Murata Infineon firmware binaries: "
	install -d ${D}${base_libdir}/firmware/cypress
	install -d ${D}${base_libdir}/firmware/brcm
	install -d ${D}${sbindir}

	# Install Bluetooth patch *.HCD file
	# For Murata 2AE (LBEE5PK2AE-564)
	install -m 444 ${S}/cyw-bt-patch/BCM4373A0_001.001.025.0103.0155.FCC.CE.2AE.hcd ${D}${base_libdir}/firmware/brcm/BCM4373A0_FCC.CE.hcd
	install -m 444 ${S}/cyw-bt-patch/BCM4373A0_001.001.025.0103.0156.JRL.2AE.hcd ${D}${base_libdir}/firmware/brcm/BCM4373A0_JRL.hcd

	# Install WLAN firmware file (*.bin) and Regulatory binary file (*.clm_blob)
	# For Murata 2AE (LBEE5PK2AE-564)
	install -m 444 ${S}/cyw-fmac-fw/cyfmac4373-sdio.2AE.bin ${D}${base_libdir}/firmware/cypress/cyfmac4373-sdio.bin
	install -m 444 cyfmac4373-sdio_US.clm_blob ${D}${base_libdir}/firmware/cypress/cyfmac4373-sdio_US.clm_blob
	install -m 444 cyfmac4373-sdio_World.clm_blob ${D}${base_libdir}/firmware/cypress/cyfmac4373-sdio_World.clm_blob

	# Install NVRAM files (*.txt)
	# For Murata 2AE (LBEE5PK2AE-564)
	install -m 444 ${S}/cyw-fmac-nvram/cyfmac4373-sdio.2AE.txt ${D}${base_libdir}/firmware/cypress/cyfmac4373-sdio.txt

	# Install WLAN client utility binary based on 32-bit/64-bit arch
	if [ ${TARGET_ARCH} = "aarch64" ]; then
		install -m 755 ${S}/cyw-fmac-utils-imx64/wl ${D}${sbindir}
	else
		install -m 755 ${S}/cyw-fmac-utils-imx32/wl ${D}${sbindir}
	fi

	if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
		# Install systemd unit files
		install -d ${D}${systemd_unitdir}/system/
		install -m 0644 ${WORKDIR}/cyw4373-autocountry.service ${D}${systemd_unitdir}/system/cyw4373-autocountry.service
	fi

	install -d ${D}${sysconfdir}/init.d/
	install -m 0755 ${WORKDIR}/cyw4373-autocountry ${D}${sysconfdir}/cyw4373-autocountry
	ln -sf /etc/cyw4373-autocountry ${D}${sysconfdir}/init.d/cyw4373-autocountry
}

inherit update-rc.d systemd

INITSCRIPT_PACKAGES += "${PN}-autocountry"
INITSCRIPT_NAME:${PN}-autocountry = "cyw4373-autocountry"
INITSCRIPT_PARAMS:${PN}-autocountry = "start 19 2 3 4 5 . stop 21 0 1 6 ."

SYSTEMD_PACKAGES = "${PN}-autocountry"
SYSTEMD_SERVICE:${PN}-autocountry = "cyw4373-autocountry.service"

PACKAGES =+ " \
    ${PN}-mfgtest \
    ${PN}-autocountry \
"

FILES:${PN} = " \
    ${base_libdir}/firmware \
"

FILES:${PN}-mfgtest = " \
    ${sbindir} \
"

FILES:${PN}-autocountry = " \
    ${sysconfdir}/cyw4373-autocountry \
    ${sysconfdir}/init.d/cyw4373-autocountry \
    ${systemd_unitdir}/system/cyw4373-autocountry.service \
"

RDEPENDS:${PN}:append = " ${PN}-autocountry"
RDEPENDS:${PN}-autocountry:append = " ${PN}-mfgtest"

INSANE_SKIP:${PN} += "build-deps"
INSANE_SKIP:${PN} += "file-rdeps"

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccmp1)"
