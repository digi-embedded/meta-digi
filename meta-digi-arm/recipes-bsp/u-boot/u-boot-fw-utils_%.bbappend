# Copyright (C) 2015 Digi International

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI += " \
    file://0002-fw_env-add-support-to-unlock-emmc-boot-partition.patch \
    file://fw_env.config \
"

# We do not have a CC6 defconfig in this version of u-boot, so just use the generic
# sandbox defconfig, which is enough to build the Linux user-space tool (fw_printenv)
UBOOT_CONFIG = "sandbox"
UBOOT_CONFIG[sandbox] = "sandbox_defconfig"

do_install_append() {
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

COMPATIBLE_MACHINE = "(ccimx6$)"
