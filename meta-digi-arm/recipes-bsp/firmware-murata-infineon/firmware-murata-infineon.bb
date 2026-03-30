# Copyright (C) 2022-2026, Digi International Inc.

SUMMARY = "Murata Infineon firmware binaries"
SECTION = "base"
LICENSE = "CYPRESS-EULA"
LIC_FILES_CHKSUM = "file://${S}/cyw-bt-patch/LICENCE.cypress;md5=cbc5f665d04f741f1e006d2096236ba7"

SRC_URI = " \
    git://github.com/Infineon/ifx-linux-firmware;protocol=http;branch=master;destsuffix=ifx-linux-firmware-longma;name=ifx-linux-firmware-longma \
    git://github.com/murata-wireless/cyw-fmac-fw;protocol=http;branch=longma;destsuffix=cyw-fmac-fw;name=cyw-fmac-fw \
    git://github.com/murata-wireless/cyw-fmac-nvram;protocol=http;branch=longma;destsuffix=cyw-fmac-nvram;name=cyw-fmac-nvram \
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
    file://mbt \
"

SRCREV_ifx-linux-firmware-longma="f24790e6fa2f05a0f974236bed7da7fa493b9ad2"
SRCREV_cyw-fmac-fw="8cdb1886852e0b5f9876654619a8371b952bf248"
SRCREV_cyw-fmac-nvram="411c87d4cf924a1a5415273265fd54d7d7d4044f"
SRCREV_cyw-bt-patch="64ac86708253e12d7089cf75ef8dcc9b30594958"
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
	install -m 444 ${S}/cyw-bt-patch/BCM4373A0_001.001.025.0103.0155.FCC.CE.2AE.hcd ${D}${base_libdir}/firmware/brcm/
	install -m 444 ${S}/cyw-bt-patch/BCM4373A0_001.001.025.0103.0156.JRL.2AE.hcd ${D}${base_libdir}/firmware/brcm/
	ln -sf BCM4373A0_001.001.025.0103.0155.FCC.CE.2AE.hcd ${D}${base_libdir}/firmware/brcm/BCM4373A0_FCC.hcd
	ln -sf BCM4373A0_001.001.025.0103.0155.FCC.CE.2AE.hcd ${D}${base_libdir}/firmware/brcm/BCM4373A0_CE.hcd
	ln -sf BCM4373A0_001.001.025.0103.0156.JRL.2AE.hcd ${D}${base_libdir}/firmware/brcm/BCM4373A0_JRL.hcd

	# Install WLAN firmware file (*.bin) and Regulatory binary file (*.clm_blob)
	# For Murata 2AE (LBEE5PK2AE-564)
	install -m 444 ${S}/ifx-linux-firmware-longma/firmware/cyfmac4373-sdio.industrial.bin ${D}${base_libdir}/firmware/cypress/cyfmac4373-sdio.bin
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
	# For Murata 2FY (LBEE5HY2FY)
	install -m 444 ${S}/cyw-bt-patch/CYW55500A1_001.002.032.0040.0033.FCC.2FY.2GY.hcd ${D}${base_libdir}/firmware/brcm/
	install -m 444 ${S}/cyw-bt-patch/CYW55500A1_001.002.032.0040.0032.CE.JP.2FY.2GY.hcd ${D}${base_libdir}/firmware/brcm/
	ln -sf CYW55500A1_001.002.032.0040.0033.FCC.2FY.2GY.hcd ${D}${base_libdir}/firmware/brcm/CYW55500A1_FCC.hcd
	ln -sf CYW55500A1_001.002.032.0040.0032.CE.JP.2FY.2GY.hcd ${D}${base_libdir}/firmware/brcm/CYW55500A1_CE.hcd
	ln -sf CYW55500A1_001.002.032.0040.0032.CE.JP.2FY.2GY.hcd ${D}${base_libdir}/firmware/brcm/CYW55500A1_JP.hcd

	# Install WLAN firmware file (*.bin) and Regulatory binary file (*.clm_blob)
	# For Murata 2FY (LBEE5HY2FY)
	install -m 444 ${S}/ifx-linux-firmware-longma/firmware/cyfmac55500-sdio.trxse ${D}${base_libdir}/firmware/cypress/cyfmac55500-sdio.trxse
	install -m 444 ${S}/cyw-fmac-fw/cyfmac55500-sdio.2FY.STAIndoor.clm_blob  ${D}/${base_libdir}/firmware/cypress/
	ln -sf cyfmac55500-sdio.2FY.STAIndoor.clm_blob ${D}/${base_libdir}/firmware/cypress/cyfmac55500-sdio_US.clm_blob

	# Install NVRAM files (*.txt)
	# For Murata 2FY (LBEE5HY2FY)
	install -m 444 ${S}/cyw-fmac-nvram/cyfmac55500-sdio.2FY.txt ${D}${base_libdir}/firmware/cypress/cyfmac55500-sdio.txt

	# Install Manufacturing Bluetooth Test tool (MBT)
	install -m 755 mbt ${D}${sbindir}
}

do_install:append:ccimx95 () {
	install -d ${D}${base_libdir}/firmware/cypress
	install -d ${D}${base_libdir}/firmware/brcm

	# Install Bluetooth patch *.HCD file
	# For Murata 2EC (LBEE5XV2EC)
	install -m 444 ${S}/cyw-bt-patch/CYW55560A1_001.002.087.0269.0100.FCC.2EA.sAnt.hcd ${D}${base_libdir}/firmware/brcm/
	install -m 444 ${S}/cyw-bt-patch/CYW55560A1_001.002.087.0269.0106.EU.JP.2EA.sAnt.hcd ${D}${base_libdir}/firmware/brcm/
	ln -sf CYW55560A1_001.002.087.0269.0100.FCC.2EA.sAnt.hcd ${D}${base_libdir}/firmware/brcm/CYW55560A1_FCC.hcd
	ln -sf CYW55560A1_001.002.087.0269.0106.EU.JP.2EA.sAnt.hcd ${D}${base_libdir}/firmware/brcm/CYW55560A1_CE.hcd
	ln -sf CYW55560A1_001.002.087.0269.0106.EU.JP.2EA.sAnt.hcd ${D}${base_libdir}/firmware/brcm/CYW55560A1_JP.hcd

	# Install WLAN firmware file (*.bin) and Regulatory binary file (*.clm_blob)
	# For Murata 2EC (LBEE5XV2EC)
	install -m 444 ${S}/ifx-linux-firmware-longma/firmware/cyfmac55572-sdio.trxse ${D}${base_libdir}/firmware/cypress/cyfmac55572-sdio.trxse
	install -m 444 ${S}/cyw-fmac-fw/cyfmac55572-sdio.2EA.clm_blob_STAIndoor  ${D}/${base_libdir}/firmware/cypress/
	ln -sf cyfmac55572-sdio.2EA.clm_blob_STAIndoor ${D}/${base_libdir}/firmware/cypress/cyfmac55572-sdio_US.clm_blob

	# Install NVRAM files (*.txt)
	# For Murata 2EC (LBEE5XV2EC)
	install -m 444 ${S}/cyw-fmac-nvram/cyfmac5557x-pcie_sdio.sant.2EA_2EC.txt ${D}${base_libdir}/firmware/cypress/cyfmac55572-sdio.txt
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
COMPATIBLE_MACHINE = "(ccmp1|ccmp2|ccimx95)"
