# Copyright (C) 2016-2023, Digi International Inc.

SUMMARY = "Trustfence initramfs required files"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI = "file://trustfence-initramfs-init"

S = "${WORKDIR}"

do_install() {
	install -m 0755 trustfence-initramfs-init ${D}/init
}

# Do not create debug/devel packages
PACKAGES = "${PN}"

FILES:${PN} = "/"

# Runtime packages used in 'trustfence-initramfs-init'
RDEPENDS:${PN} = " \
    libubootenv-bin \
    cryptsetup \
    trustfence-tool \
    util-linux-findfs \
"

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccimx6|ccimx8m|ccimx8x|ccimx93)"
