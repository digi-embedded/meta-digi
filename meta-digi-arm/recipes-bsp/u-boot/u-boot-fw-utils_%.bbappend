# Copyright (C) 2015-2017 Digi International

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI += " \
    file://fw_env.config \
"

UBOOT_FW_UTILS_PATCHES = " \
    file://0001-tools-env-implement-support-for-environment-encrypti.patch \
    file://0002-Implement-U-Boot-environment-access-functions.patch \
    file://0003-fw_env-add-support-to-unlock-emmc-boot-partition.patch \
"

# Patches from 'meta-swupdate' touch the same files than ours, so we need to
# force that our patches are applied later. As our layer has more priority than
# 'meta-swupdate' we need to do the changes to SRC_URI in an anonymous python
# function instead of a normal '_append' to the SRC_URI variable.
python() {
    ufw_patches = d.getVar('UBOOT_FW_UTILS_PATCHES', True)
    if ufw_patches:
        src_uri = d.getVar('SRC_URI', True)
        d.setVar('SRC_URI', src_uri + ufw_patches)
}

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
