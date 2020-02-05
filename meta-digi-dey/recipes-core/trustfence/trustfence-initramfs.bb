# Copyright (C) 2016-2020 Digi International Inc.

SUMMARY = "Trustfence initramfs required files"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI = " \
    ${@bb.utils.contains('STORAGE_MEDIA', 'mmc', 'file://trustfence-initramfs-init_mmc', '', d)} \
    ${@bb.utils.contains('STORAGE_MEDIA', 'mtd', 'file://trustfence-initramfs-init_mtd', '', d)} \
"

S = "${WORKDIR}"

do_install() {
	if [ "${STORAGE_MEDIA}" = "mmc" ]; then
		install -m 0755 trustfence-initramfs-init_mmc ${D}/init
	else
		install -m 0755 trustfence-initramfs-init_mtd ${D}/init
	fi
}

# Do not create debug/devel packages
PACKAGES = "${PN}"

FILES_${PN} = "/"

# Runtime packages used in 'trustfence-initramfs-init'
RDEPENDS_${PN} = " \
    ${@bb.utils.contains('STORAGE_MEDIA', 'mmc', 'cryptsetup', '', d)} \
    ${@bb.utils.contains('STORAGE_MEDIA', 'mtd', 'mtd-utils-ubifs', '', d)} \
    trustfence-tool \
    util-linux-findfs \
    wipe \
    u-boot-fw-utils \
"

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccimx6|ccimx6ul|ccimx8x|ccimx8m)"
