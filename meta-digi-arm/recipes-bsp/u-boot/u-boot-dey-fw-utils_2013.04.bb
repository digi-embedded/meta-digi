DESCRIPTION = "U-boot bootloader fw_printenv/setenv utils"
LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://COPYING;md5=1707d6db1d42237583f50183a5651ecb"
SECTION = "bootloader"

include u-boot-dey-rev_${PV}.inc

SRC_URI += " \
    file://0001-fw_env-add-support-to-unlock-emmc-boot-partition.patch \
    file://fw_env.config \
"

S = "${WORKDIR}/git"

#
# In a u-boot multiconfig case, UBOOT_MACHINE has multiple values. Using
# parallel build leads to build failures:
#
#   ln: failed to create symbolic link 'asm/arch/arch-mx6': File exists
#   ln: failed to create symbolic link 'asm/arch': No such file or directory
#
# Without parallel make, UBOOT_MACHINE's last entry is used to configure uboot
#
PARALLEL_MAKE = ""

EXTRA_OEMAKE = 'HOSTCC="${CC}" HOSTSTRIP="true"'

inherit uboot-config

do_compile() {
	oe_runmake ${UBOOT_MACHINE}
	oe_runmake env
}

do_install() {
	install -d ${D}${base_sbindir} ${D}${sysconfdir}
	install -m 0755 ${S}/tools/env/fw_printenv ${D}${base_sbindir}/fw_printenv
	ln -sf fw_printenv ${D}${base_sbindir}/fw_setenv
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

PACKAGE_ARCH = "${MACHINE_ARCH}"

COMPATIBLE_MACHINE = "(ccimx6)"
