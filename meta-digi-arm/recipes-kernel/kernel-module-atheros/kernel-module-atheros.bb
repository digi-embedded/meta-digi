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
SRCREV_internal = "50dafb5890180cf33fdb42919c3e6f591d0cd2ea"
SRCREV = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRCREV_internal}', '${SRCREV_external}', d)}"

SRC_URI_external = "${DIGI_GITHUB_GIT}/atheros.git;protocol=git;nobranch=1"
SRC_URI_internal = "${DIGI_GIT}linux-modules/atheros.git;protocol=git;nobranch=1"
SRC_URI  = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRC_URI_internal}', '${SRC_URI_external}', d)}"
SRC_URI += " \
    file://atheros-pre-up \
    file://Makefile \
    ${@base_conditional('IS_KERNEL_2X', '1' , '', 'file://0001-atheros-convert-NLA_PUT-macros.patch', d)} \
    ${@base_conditional('IS_KERNEL_2X', '1' , '', 'file://0002-atheros-update-renamed-struct-members.patch', d)} \
"

# MX6 wireless calibration file and post-down script
SRC_URI_append_ccimx6 = " \
    file://Digi_6203-6233-US.bin \
    file://atheros-post-down \
"

S = "${WORKDIR}/git"

EXTRA_OEMAKE += "DEL_PLATFORM=${MACHINE} KLIB_BUILD=${STAGING_KERNEL_DIR}"

do_configure_prepend() {
	cp ${WORKDIR}/Makefile ${S}/
}

do_configure_prepend_ccimx6() {
	cp ${WORKDIR}/Digi_6203-6233-US.bin ${S}/Firmware_Package/target/AR6003/hw2.1.1/
}

do_install_append() {
	install -d ${D}${sysconfdir}/network/if-pre-up.d
	install -m 0755 ${WORKDIR}/atheros-pre-up ${D}${sysconfdir}/network/if-pre-up.d/atheros
	install -d ${D}${sysconfdir}/modprobe.d
	cat >> ${D}${sysconfdir}/modprobe.d/atheros.conf <<-_EOF_
		install ath6kl_sdio true
		options ath6kl_sdio ath6kl_p2p=1 softmac_enable=1
	_EOF_
}

do_install_append_ccimx6() {
	install -d ${D}${sysconfdir}/network/if-post-down.d
	install -m 0755 ${WORKDIR}/atheros-post-down ${D}${sysconfdir}/network/if-post-down.d/atheros
}

FILES_${PN} += " \
    ${base_libdir}/firmware/ \
    ${sysconfdir}/modprobe.d/ \
    ${sysconfdir}/network/ \
"

COMPATIBLE_MACHINE = "(ccardimx28|ccimx6)"
