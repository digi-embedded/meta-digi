# Copyright (C) 2016 Digi International.

SUMMARY = "Qualcomm's wireless driver for qca6564"
DESCRIPTION = "qcacld-2.0 module.bbclass mechanism."
LICENSE = "ISC"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/ISC;md5=f3b90e78ea0cffb20bf5cca7947a896d"
DEPENDS = "virtual/kernel"

inherit module

CAF_MIRROR = "git://codeaurora.org/quic/la/platform/vendor/qcom-opensource/wlan/qcacld-2.0"
PV = "v4.2.79.63"
SRCBRANCH = "caf-wlan/QCA6564_LE_1.0.3_LA.4.2.2.3"
SRCREV = "b0ae2aa45bbba54600b537e90cb1aca34f2d1a13"

SRC_URI = " \
    ${CAF_MIRROR};destsuffix=${PV};branch=${SRCBRANCH} \
    file://qualcomm-pre-up \
    file://modprobe-qualcomm.conf \
    file://0001-qcacld-Fix-compiling-errors-when-BUILD_DEBUG_VERSION.patch \
    file://0002-Update-cfg80211_vendor_event_alloc-call-for-newer-ke.patch \
    file://0003-wlan_hdd_main-Update-cfg80211_ap_stopped-to-nl80211_.patch \
    file://0004-qcacld-2.0-remove-unused-code.patch \
    file://0005-Including-header-file-for-regulatory_hint_user.patch \
    file://0006-Updating-calls-to-alloc_netdev_mq.patch \
    file://0007-wlan_hdd_cfg80211-update-cfg80211_inform_bss-params-.patch \
    file://0008-wlan_hdd_p2p-Update-call-to-cfg80211_rx_mgmt-for-dif.patch \
    file://0009-linux_ac-Fix-for-f_dentry.patch \
    file://0010-native_sdio-src-hif-Do-not-call-to-HIGH-SPEED-functi.patch \
    file://0011-osdep_adf.h-fix-for-undefined-ath_sysctl_pktlog_size.patch \
    file://0012-Kbuild-Add-compilation-flag-based-on-kernel-support.patch \
    file://0013-Kbuild-do-not-create-an-auxiliar-p2p-on-init.patch \
    file://0014-Kbuild-do-not-compile-the-DEBUG-version-inconditiona.patch \
    file://0015-Kbuild-Group-most-of-the-relevant-DEBUG-options.patch \
    file://0016-wlan_hdd_cfg80211-fix-missing-ifdef-clause.patch \
    file://0017-Add-.gitignore-rules.patch \
    file://0018-wlan_hdd_main-initialize-all-adapter-completion-vari.patch \
    file://0019-qcacld-Indicate-disconnect-event-to-upper-layers.patch \
"

S = "${WORKDIR}/${PV}"

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
