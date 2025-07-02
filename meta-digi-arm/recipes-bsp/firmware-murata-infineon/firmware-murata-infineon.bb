# Copyright (C) 2022-2025, Digi International Inc.

SUMMARY = "Murata Infineon firmware binaries"
SECTION = "base"
LICENSE = "CYPRESS-EULA"
LIC_FILES_CHKSUM = "file://${S}/cyw-bt-patch/LICENCE.cypress;md5=cbc5f665d04f741f1e006d2096236ba7"

SRC_URI = " \
    https://github.com/Infineon/ifx-linux-firmware/archive/refs/tags/release-v6.1.97-2025_0219.tar.gz;destsuffix=cyw-fmac-fw-ifx;name=cyw-fmac-fw-ifx \
    git://github.com/murata-wireless/cyw-fmac-fw;protocol=http;branch=jaculus;destsuffix=cyw-fmac-fw;name=cyw-fmac-fw \
    git://github.com/murata-wireless/cyw-fmac-nvram;protocol=http;branch=jaculus;destsuffix=cyw-fmac-nvram;name=cyw-fmac-nvram \
    git://github.com/murata-wireless/cyw-bt-patch;protocol=http;branch=master;destsuffix=cyw-bt-patch;name=cyw-bt-patch \
    git://github.com/murata-wireless/cyw-fmac-utils-imx32;protocol=http;branch=master;destsuffix=cyw-fmac-utils-imx32;name=cyw-fmac-utils-imx32 \
    git://github.com/murata-wireless/cyw-fmac-utils-imx64;protocol=http;branch=master;destsuffix=cyw-fmac-utils-imx64;name=cyw-fmac-utils-imx64 \
    file://cyw4373-autocountry \
    file://cyw4373-autocountry.service \
"

SRC_URI:append:ccmp1 = " \
    file://cyfmac4373-sdio_US.clm_blob \
    file://cyfmac4373-sdio_World.clm_blob \
"

SRC_URI:append:ccmp2 = " \
    file://cyfmac55500-sdio.txt \
    file://mbt \
"

SRC_URI[cyw-fmac-fw-ifx.sha256sum]="1ff021a1b3ef1608f5c0ad4b39fec8e0f5cb8c204d178f1de0f8215d06abc8d9"
SRCREV_cyw-fmac-fw="a5cb86a5d11192ba6e7738f82b4d2dc9eeeca679"
SRCREV_cyw-fmac-nvram="146d1438372b6c4857f92b8769b91c1801d3ede2"
SRCREV_cyw-bt-patch="23de75a4e5384d16e8478f668b769b0d24ede0de"
SRCREV_cyw-fmac-utils-imx32="dad9ed86bf6691910197bc91d42a45ea8175180c"
SRCREV_cyw-fmac-utils-imx64="368bd9a4163e115468d79c238192b41f6266c523"

SRCREV_FORMAT = "cyw-fmac-fw_cyw-fmac-nvram_cyw-bt-patch_cyw-fmac-utils-imx32_cyw-fmac-utils-imx64"

SRCREV_default = "${AUTOREV}"

S = "${WORKDIR}"

DEPENDS = "libnl"

INSANE_SKIP:append:stm32mp1common = " 32bit-time"

do_install () {
	bbnote "Installing Murata Infineon firmware binaries: "
	install -d ${D}${sbindir}

	if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
		# Install systemd unit files
		install -d ${D}${systemd_unitdir}/system/
		install -m 0644 ${WORKDIR}/cyw4373-autocountry.service ${D}${systemd_unitdir}/system/cyw4373-autocountry.service
	fi

	install -d ${D}${sysconfdir}/init.d/

	# Install autocountry service
	install -m 0755 ${WORKDIR}/cyw4373-autocountry ${D}${sysconfdir}/cyw4373-autocountry
	ln -sf /etc/cyw4373-autocountry ${D}${sysconfdir}/init.d/cyw4373-autocountry

	# Install WLAN client utility binary based on 32-bit/64-bit arch
	if [ ${TARGET_ARCH} = "aarch64" ]; then
		install -m 755 ${S}/cyw-fmac-utils-imx64/wl ${D}${sbindir}
	else
		install -m 755 ${S}/cyw-fmac-utils-imx32/wl ${D}${sbindir}
	fi
}

do_install:append:ccmp1 () {
	install -d ${D}${base_libdir}/firmware/cypress
	install -d ${D}${base_libdir}/firmware/brcm

	# Install Bluetooth patch *.HCD file
	# For Murata 2AE (LBEE5PK2AE-564)
	install -m 444 ${S}/cyw-bt-patch/BCM4373A0_001.001.025.0103.0155.FCC.CE.2AE.hcd ${D}${base_libdir}/firmware/brcm/BCM4373A0_FCC.CE.hcd
	install -m 444 ${S}/cyw-bt-patch/BCM4373A0_001.001.025.0103.0156.JRL.2AE.hcd ${D}${base_libdir}/firmware/brcm/BCM4373A0_JRL.hcd

	# Install WLAN firmware file (*.bin) and Regulatory binary file (*.clm_blob)
	# For Murata 2AE (LBEE5PK2AE-564)
	install -m 444 ${S}/ifx-linux-firmware-release-v6.1.97-2025_0219/firmware/cyfmac4373-sdio.industrial.bin ${D}${base_libdir}/firmware/cypress/cyfmac4373-sdio.bin
	install -m 444 cyfmac4373-sdio_US.clm_blob ${D}${base_libdir}/firmware/cypress/cyfmac4373-sdio_US.clm_blob
	install -m 444 cyfmac4373-sdio_World.clm_blob ${D}${base_libdir}/firmware/cypress/cyfmac4373-sdio_World.clm_blob

	# Install NVRAM files (*.txt)
	# For Murata 2AE (LBEE5PK2AE-564)
	install -m 444 ${S}/cyw-fmac-nvram/cyfmac4373-sdio.2AE.txt ${D}${base_libdir}/firmware/cypress/cyfmac4373-sdio.txt
}

do_install:append:ccmp2 () {
	install -d ${D}${base_libdir}/firmware/cypress
	install -d ${D}${base_libdir}/firmware/brcm

	# Install Bluetooth patch *.HCD file
	# For Murata 2GY (LBEE5HY2GY) and Murata 2FY (LBEE5HY2FY)
	install -m 444 ${S}/cyw-bt-patch/CYW55500A1_001.002.032.0040.0033.2FY.hcd ${D}${base_libdir}/firmware/brcm/CYW55500A1.hcd

	# Install WLAN firmware file (*.bin) and Regulatory binary file (*.clm_blob)
	# For Murata 2GY (LBEE5HY2GY) and Murata 2FY (LBEE5HY2FY)
	install -m 444 ${S}/ifx-linux-firmware-release-v6.1.97-2025_0219/firmware/cyfmac55500-sdio.trxse ${D}${base_libdir}/firmware/cypress/cyfmac55500-sdio.trxse
	install -m 444 ${S}/cyw-fmac-fw/cyfmac55500-sdio.2FY.clm_blob ${D}${base_libdir}/firmware/cypress/cyfmac55500-sdio_US.clm_blob

	# Install NVRAM files (*.txt)
	# For Murata 2GY (LBEE5HY2GY) and Murata 2FY (LBEE5HY2FY)
	install -m 444 ${S}/cyfmac55500-sdio.txt ${D}${base_libdir}/firmware/cypress/cyfmac55500-sdio.txt

	# Install Manufacturing Bluetooth Test tool (MBT)
	install -m 755 mbt ${D}${sbindir}
}

inherit update-rc.d systemd

INITSCRIPT_PACKAGES += "${PN}-autocountry"
INITSCRIPT_NAME:${PN}-autocountry = "cyw4373-autocountry"
INITSCRIPT_PARAMS:${PN}-autocountry = "start 19 2 3 4 5 . stop 21 0 1 6 ."

SYSTEMD_SERVICE:${PN}-autocountry = "cyw4373-autocountry.service"
SYSTEMD_PACKAGES = "${PN}-autocountry"

PACKAGES =+ " \
    ${PN}-mfgtest \
    ${PN}-autocountry \
"

FILES:${PN} = " \
    ${base_libdir}/firmware \
"

FILES:${PN}-mfgtest = " \
    ${sbindir}/wl \
"

FILES:${PN}-autocountry = " \
    ${sysconfdir}/cyw4373-autocountry \
    ${sysconfdir}/init.d/cyw4373-autocountry \
    ${systemd_unitdir}/system/cyw4373-autocountry.service \
"

FILES:${PN}:append:ccmp2 = " \
    ${sbindir}/mbt \
"

RDEPENDS:${PN}:append:ccmp1 = " ${PN}-autocountry"
RDEPENDS:${PN}-autocountry:append = " ${PN}-mfgtest"

INSANE_SKIP:${PN} += "build-deps"
INSANE_SKIP:${PN} += "file-rdeps"

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccmp1|ccmp2)"
