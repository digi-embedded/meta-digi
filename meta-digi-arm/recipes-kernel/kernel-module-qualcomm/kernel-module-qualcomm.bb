# Copyright (C) 2016,2017 Digi International.

SUMMARY = "Qualcomm's wireless driver for qca6564"
DESCRIPTION = "qcacld-2.0 module"
LICENSE = "ISC"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/ISC;md5=f3b90e78ea0cffb20bf5cca7947a896d"

PV = "v4.2.80.63"

SRCBRANCH = "dey-2.2/maint"
SRCREV = "${AUTOREV}"

QCOM_GIT_URI = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${DIGI_MTK_GIT}linux/qcacld-2.0.git;protocol=ssh', '${DIGI_GITHUB_GIT}/qcacld-2.0.git', d)}"

SRC_URI = " \
    ${QCOM_GIT_URI};branch=${SRCBRANCH} \
"

SRC_URI_append = " \
    file://qualcomm-pre-up \
    file://modprobe-qualcomm.conf \
"

S = "${WORKDIR}/git"

inherit module

EXTRA_OEMAKE += "CONFIG_CLD_HL_SDIO_CORE=y CONFIG_LINUX_QCMBR=y WLAN_OPEN_SOURCE=1"
# Explicity state it is not a QC platform, if not the driver will try to remap
# memory that is not allowed in ARMv6 (kernel commit
# 309caa9cc6ff39d261264ec4ff10e29489afc8f8)
EXTRA_OEMAKE += "CONFIG_NON_QC_PLATFORM=y"
# Flag to compile the debug version (1 - enabled, rest of values - disabled)
EXTRA_OEMAKE += "BUILD_DEBUG_VERSION=0"

do_compile_prepend() {
	export BUILD_VER=${PV}
}

do_install_append() {
	install -d ${D}${sysconfdir}/network/if-pre-up.d
	install -m 0755 ${WORKDIR}/qualcomm-pre-up ${D}${sysconfdir}/network/if-pre-up.d/qualcomm
	install -d ${D}${sysconfdir}/modprobe.d
	install -m 0644 ${WORKDIR}/modprobe-qualcomm.conf ${D}${sysconfdir}/modprobe.d/qualcomm.conf
}

FILES_${PN} += " \
    ${sysconfdir}/network/if-pre-up.d/qualcomm \
    ${sysconfdir}/modprobe.d/qualcomm.conf \
"

COMPATIBLE_MACHINE = "(ccimx6ul)"
