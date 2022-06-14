# Copyright (C) 2016-2022 Digi International.

SUMMARY = "Qualcomm's wireless driver for qca65xx"
DESCRIPTION = "qcacld-3.0 module"
LICENSE = "ISC"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/ISC;md5=f3b90e78ea0cffb20bf5cca7947a896d"

# Reference Qualcomm tag/version
PV = "v5.2.0.237G"

QCACLD_SRCBRANCH = "wlan-cld3.driver.lnx.2.0.r51-rel"
QCA_WIFI_HOST_CMN_SRCBRANCH = "wlan-cmn.driver.lnx.2.0.r51-rel"
FW_API_SRCBRANCH = "wlan-api.lnx.1.0.c21.2"
MDM_INIT_SRCBRANCH = "wlan-tools.lnx.1.0.c21.2"

SRC_URI = " \
    git://git.codelinaro.org/clo/la/platform/vendor/qcom-opensource/wlan/qcacld-3.0.git;protocol=https;branch=${QCACLD_SRCBRANCH};name=qcacld \
    git://git.codelinaro.org/clo/la/platform/vendor/qcom-opensource/wlan/qca-wifi-host-cmn.git;protocol=https;branch=${QCA_WIFI_HOST_CMN_SRCBRANCH};destsuffix=qca-wifi-host-cmn;name=qca-wifi-host \
    git://git.codelinaro.org/clo/la/platform/vendor/qcom-opensource/wlan/fw-api.git;protocol=https;branch=${FW_API_SRCBRANCH};destsuffix=fw-api;name=fw-api \
    git://git.codelinaro.org/clo/le/qcom-opensource/mdm-init.git;protocol=https;branch=${MDM_INIT_SRCBRANCH};destsuffix=mdm-init;name=mdm-init \
    file://0001-qcacld-3.0-disable-warnings-as-errors.patch \
    file://0002-qcacld-3.0-support-ROME-SDIO-build.patch \
    file://0003-qcacld-3.0-cfg-try-to-get-MACs-from-device-tree-entr.patch \
    file://0004-qcacld-3.0-Add-proper-check-to-include-qcom-iommu-ut.patch \
    file://0001-qca-wifi-host-cmn-fix-buid-issue-for-Rome-SDIO-inter.patch;patchdir=${WORKDIR}/qca-wifi-host-cmn; \
    file://0002-qca-wifi-host-cmn-fix-build-issue-enabling-debug-for.patch;patchdir=${WORKDIR}/qca-wifi-host-cmn; \
    file://0003-qca-wifi-host-cmn-fix-panic_notifier_list-undeclared.patch;patchdir=${WORKDIR}/qca-wifi-host-cmn; \
"

# Tag 'CHSS.LNX_FSL.5.0-01200-QCA6574AUARMSDIOHZ' in all repos
SRCREV_qcacld = "f1dae2986ae58c68ea740e2c505be9c369547916"
SRCREV_qca-wifi-host = "ca5e999f4f692a45ae9974a7ad92726deaf7497f"
SRCREV_fw-api = "62b94874003ef7aced22bba1a076c1e4b5d5a9a9"
SRCREV_mdm-init = "3fb3bcb9f054eeeb1083bd4f6dbaf733061c5af3"

inherit module

DEPENDS = "virtual/kernel"

# Selects whether the interface is SDIO or PCI
QUALCOMM_WIFI_INTERFACE ?= "sdio"
QUALCOMM_WIFI_INTERFACE:ccimx8x = "pci"

WLAN_CONFIG_INI = "${@oe.utils.conditional('QUALCOMM_WIFI_INTERFACE', 'sdio' , \
                                           'QCA6574AU.LE.2.2.1_Rome_SDIO_qcacld-3.0.ini', \
                                           'QCA6574AU.LE.2.2.1_Rome_PCIe_qcacld-3.0.ini', d)}"

SRC_URI:append = " \
    file://81-qcom-wifi.rules \
    file://qualcomm.sh \
"

FILES_SDIO = " \
    file://modprobe-qualcomm.conf \
"

SRC_URI:append = "${@oe.utils.conditional('QUALCOMM_WIFI_INTERFACE', 'sdio' , '${FILES_SDIO}', '', d)}"

S = "${WORKDIR}/git"

WLAN_MODULE_NAME ?= "wlan"

EXTRA_OEMAKE += "CONFIG_WLAN_FEATURE_11W=y \
                 CONFIG_LINUX_QCMBR=y \
                 CONFIG_QCA_CLD_WLAN_PROFILE=qca6174 \
                 CONFIG_WLAN_DISABLE_EXPORT_SYMBOL=y \
                 MODNAME=${WLAN_MODULE_NAME} \
"

# Flags for SDIO interface with wifi
FLAGS_SDIO = "CONFIG_CLD_HL_SDIO_CORE=y"
EXTRA_OEMAKE += "${@oe.utils.conditional('QUALCOMM_WIFI_INTERFACE', 'sdio' , '${FLAGS_SDIO}', '', d)}"

# Flags for PCI interface with wifi
FLAGS_PCI = "CONFIG_ROME_IF=pci"
EXTRA_OEMAKE += "${@oe.utils.conditional('QUALCOMM_WIFI_INTERFACE', 'pci' , '${FLAGS_PCI}', '', d)}"

# Flag to compile the debug version (y - enabled, n - disabled)
EXTRA_OEMAKE += "BUILD_DEBUG_VERSION=n"

# Flag to define the maximum vdevs interfaces
EXTRA_OEMAKE += "CONFIG_WLAN_MAX_VDEVS=4"

do_compile:prepend() {
	export BUILD_VER=${PV}
}

do_install:append() {
	if [ "${QUALCOMM_WIFI_INTERFACE}" = "sdio" ]; then
		install -d ${D}${sysconfdir}/modprobe.d
		install -m 0644 ${WORKDIR}/modprobe-qualcomm.conf ${D}${sysconfdir}/modprobe.d/qualcomm.conf
	fi

	install -d ${D}${base_libdir}/firmware/wlan/
	install -m 0644 ${WORKDIR}/mdm-init/wlan_standalone/${WLAN_CONFIG_INI} ${D}${base_libdir}/firmware/wlan/qcom_cfg.ini
	# Set regulatory STRICT mode
	sed -i -e "s/gRegulatoryChangeCountry=1/gRegulatoryChangeCountry=0/g" ${D}${base_libdir}/firmware/wlan/qcom_cfg.ini
	# Disable SIFS Burst support
	sed -i -e "s/gEnableSifsBurst=1/gEnableSifsBurst=0/g" ${D}${base_libdir}/firmware/wlan/qcom_cfg.ini
	# Enable channel bonding on 2.4GHz band
	sed -i -e "/^#Channel Bonding/a gChannelBondingMode24GHz=1" ${D}${base_libdir}/firmware/wlan/qcom_cfg.ini
	# Disable 802.11d support
	sed -i -e "s/g11dSupportEnabled=1/g11dSupportEnabled=0/g" ${D}${base_libdir}/firmware/wlan/qcom_cfg.ini

	install -d ${D}${sysconfdir}/udev/scripts
	install -m 0755 ${WORKDIR}/qualcomm.sh ${D}${sysconfdir}/udev/scripts/

	install -d ${D}${sysconfdir}/udev/rules.d
	install -m 0644 ${WORKDIR}/81-qcom-wifi.rules ${D}${sysconfdir}/udev/rules.d/
}

do_install:append:ccimx6ul() {
	# Set MCS value to MCS0-7
	sed -i -e "s/gVhtTxMCS=2/gVhtTxMCS=0/g" ${D}${base_libdir}/firmware/wlan/qcom_cfg.ini
}

FILES:${PN} += " \
    ${@oe.utils.conditional('QUALCOMM_WIFI_INTERFACE', 'sdio' , '${sysconfdir}/modprobe.d/qualcomm.conf', '', d)} \
    ${sysconfdir}/udev/ \
    ${base_libdir}/firmware/wlan/qcom_cfg.ini \
"

COMPATIBLE_MACHINE = "(ccimx6$|ccimx6ul|ccimx8x|ccimx8m)"
