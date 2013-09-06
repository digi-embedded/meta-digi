# Copyright (C) 2013 Digi International.

SUMMARY = "Atheros's wireless driver"
LICENSE = "ISC"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/ISC;md5=f3b90e78ea0cffb20bf5cca7947a896d"

inherit module

PR = "r1"

# 'modprobe' from kmod package is needed to load atheros driver. The one
# from busybox does not support '--ignore-install' option.
RDEPENDS_${PN} = "kmod"

SRCREV_external = ""
SRCREV_internal = "ac3a910ebc66d0eca8f4de78b513fa3636ed9e6b"
SRCREV = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRCREV_internal}', '${SRCREV_external}', d)}"

SRC_URI_external = "${DIGI_GITHUB_GIT}/atheros.git;protocol=git"
SRC_URI_internal = "${DIGI_LOG_GIT}linux-modules/atheros.git;protocol=git"
SRC_URI  = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRC_URI_internal}', '${SRC_URI_external}', d)}"
SRC_URI += " \
    file://atheros \
    file://atheros.conf \
    file://Makefile \
"

S = "${WORKDIR}/git"

ATH_ONLY_INSTALL_FW = "${@base_version_less_or_equal('PREFERRED_VERSION_linux-dey', '2.6.35.14', '', '1', d)}"
ATH6KL_MOD = "${@base_conditional('ATH_ONLY_INSTALL_FW', '1' , 'ath6kl_core', 'ath6kl_sdio', d)}"

EXTRA_OEMAKE = "DEL_PLATFORM=${MACHINE} KLIB_BUILD=${STAGING_KERNEL_DIR} ATH_ONLY_INSTALL_FW=${ATH_ONLY_INSTALL_FW}"

do_configure_prepend() {
	cp ${WORKDIR}/Makefile ${S}/
}

do_install_append() {
	install -d ${D}${sysconfdir}/network/if-pre-up.d
	install -m 0755 ${WORKDIR}/atheros ${D}${sysconfdir}/network/if-pre-up.d/
	install -d ${D}${sysconfdir}/modprobe.d
	install -m 0644 ${WORKDIR}/atheros.conf ${D}${sysconfdir}/modprobe.d/
	echo "options ${ATH6KL_MOD} ath6kl_p2p=1 softmac_enable=1" >> ${D}${sysconfdir}/modprobe.d/atheros.conf
}

FILES_${PN} += " \
    ${base_libdir}/firmware/ \
    ${sysconfdir}/modprobe.d/ \
    ${sysconfdir}/network/ \
"

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(mxs)"
