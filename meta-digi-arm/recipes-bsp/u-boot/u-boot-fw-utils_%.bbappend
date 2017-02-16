# Copyright (C) 2015-2017 Digi International

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI += " \
    file://fw_env.config \
    file://0001-tools-env-bug-config-structs-must-be-defined-in-tool.patch \
    file://0002-tools-env-fix-config-file-loading-in-env-library.patch \
    file://0003-tools-env-implement-support-for-environment-encrypti.patch \
    file://0004-Implement-U-Boot-environment-access-functions.patch \
    file://0005-fw_env-add-support-to-unlock-emmc-boot-partition.patch \
"

# We do not have a platform defconfig in this version of u-boot, so just use the generic
# sandbox defconfig, which is enough to build the Linux user-space tool (fw_printenv)
UBOOT_CONFIG = "sandbox"
UBOOT_CONFIG[sandbox] = "sandbox_defconfig"

do_install_append() {
	install -d ${D}${includedir}/libubootenv
	install -m 0644 ${S}/tools/env/ubootenv.h ${D}${includedir}/libubootenv/
	install -m 0644 ${WORKDIR}/fw_env.config ${D}${sysconfdir}/
}

pkg_postinst_${PN}() {
	# run the postinst script on first boot
	if [ x"$D" != "x" ]; then
		exit 1
	fi
	MMCDEV="$(sed -ne 's,.*root=/dev/mmcblk\([0-9]\)p.*,\1,g;T;p' /proc/cmdline)"
	if [ -n "${MMCDEV}" ]; then
		sed -i -e "s,^/dev/mmcblk[^[:blank:]]\+,/dev/mmcblk${MMCDEV},g" /etc/fw_env.config
	fi
}

COMPATIBLE_MACHINE = "(ccimx6$|ccimx6ul)"
