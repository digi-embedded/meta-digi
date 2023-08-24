# Copyright (C) 2016-2023 Digi International Inc.

SUMMARY = "Recovery initramfs files"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

SOC_SIGN_DEPENDS = " \
    ${@oe.utils.conditional('DEY_SOC_VENDOR', 'NXP', 'trustfence-cst-native', '', d)} \
"
DEPENDS += "${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', \
		'openssl-native ' \
		'trustfence-sign-tools-native ' \
		'${SOC_SIGN_DEPENDS}', '', d)}"

SRC_URI = " \
    file://recovery-initramfs-init \
    file://swupdate.cfg \
    file://automount_block.sh \
    file://automount_mtd.sh \
    file://automount_ubi.sh \
    file://mdev.conf \
    ${@bb.utils.contains('STORAGE_MEDIA', 'mmc', 'file://mount_cryptrootfs.sh', '', d)} \
"

S = "${WORKDIR}"

do_install() {
	install -d ${D}${sysconfdir}
	install -m 0755 ${WORKDIR}/recovery-initramfs-init ${D}/init
	install -m 0644 ${WORKDIR}/swupdate.cfg ${D}${sysconfdir}
	if [ "${STORAGE_MEDIA}" = "mmc" ]; then
		install -m 0755 ${WORKDIR}/mount_cryptrootfs.sh ${D}${sysconfdir}
	fi
	install -d ${D}${base_libdir}/mdev
	install -m 0755 ${WORKDIR}/automount_block.sh ${D}${base_libdir}/mdev/automount_block.sh
	install -m 0755 ${WORKDIR}/automount_mtd.sh ${D}${base_libdir}/mdev/automount_mtd.sh
	install -m 0755 ${WORKDIR}/automount_ubi.sh ${D}${base_libdir}/mdev/automount_ubi.sh
	install -m 0644 ${WORKDIR}/mdev.conf ${D}${sysconfdir}
}

# Do not create debug/devel packages
PACKAGES = "${PN}"

FILES:${PN} = "/"

RDEPENDS:${PN}:append = "${@bb.utils.contains('STORAGE_MEDIA', 'mmc', ' cryptsetup', '', d)}"
