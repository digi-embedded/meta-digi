# Copyright (C) 2015-2020 Digi International

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI += " \
    file://${STORAGE_MEDIA}/fw_env.config \
"

UBOOT_FW_UTILS_PATCHES = " \
    file://0001-tools-env-implement-support-for-environment-encrypti.patch \
    file://0002-Implement-U-Boot-environment-access-functions.patch \
    file://0003-fw_env-add-support-to-unlock-emmc-boot-partition.patch \
    file://0004-tools-env-add-support-to-set-dynamic-location-of-env.patch \
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
	install -m 0644 ${WORKDIR}/${STORAGE_MEDIA}/fw_env.config ${D}${sysconfdir}/
}

pkg_postinst_ontarget_${PN}() {
	CONFIG_FILE="/etc/fw_env.config"
	MMCDEV="$(sed -ne 's,.*root=/dev/mmcblk\([0-9]\)p.*,\1,g;T;p' /proc/cmdline)"
	if [ -n "${MMCDEV}" ]; then
		sed -i -e "s,^/dev/mmcblk[^[:blank:]]\+,/dev/mmcblk${MMCDEV},g" ${CONFIG_FILE}
	fi

	PARTTABLE="/proc/mtd"
	MTDINDEX="$(sed -ne "s/\(^mtd[0-9]\+\):.*\<environment\>.*/\1/g;T;p" ${PARTTABLE} 2>/dev/null)"
	if [ -n "${MTDINDEX}" ]; then
		# Initialize variables for fixed offset values
		# (backwards compatible with old U-Boot)
		ENV_OFFSET="${UBOOT_ENV_OFFSET}"
		ENV_REDUND_OFFSET="${UBOOT_ENV_SIZE}"
		ENV_SIZE="${UBOOT_ENV_SIZE}"
		ERASEBLOCK=""
		NBLOCKS=""

		if [ -f "/proc/device-tree/digi,uboot,dynamic-env" ]; then
			# Update variables for dynamic environment
			# - Both copies starting at the same offset
			ENV_REDUND_OFFSET="${UBOOT_ENV_OFFSET}"
			# - Calculated erase block size
			ERASEBLOCK="$(grep "^${MTDINDEX}:" ${PARTTABLE} | awk '{printf("0x%d",$3)}')"
			# - Calculated number of blocks
			MTDSIZE="$(grep "^${MTDINDEX}:" ${PARTTABLE} | awk '{printf("0x%d",$2)}')"
			NBLOCKS="$(((MTDSIZE - UBOOT_ENV_OFFSET) / ERASEBLOCK))"
			# If a range was provided, calculate the number of
			# blocks in the range and use that number, unless they
			# exceed the total number of blocks available in the
			# whole partition.
			if [ -n "${UBOOT_ENV_RANGE}" ]; then
				RANGE_BLOCKS="$((UBOOT_ENV_RANGE / ERASEBLOCK))"
				[ "${RANGE_BLOCKS}" -lt "${NBLOCKS}" ] && NBLOCKS="${RANGE_BLOCKS}"
			fi
		fi

		# Substitute stub with configuration and calculated values
		sed -i  -e "s/##MTDINDEX##/${MTDINDEX}/g" \
			-e "s/##ENV_OFFSET##/${ENV_OFFSET}/g" \
			-e "s/##ENV_REDUND_OFFSET##/${ENV_REDUND_OFFSET}/g" \
			-e "s/##ENV_SIZE##/${ENV_SIZE}/g" \
			-e "s/##ERASEBLOCK##/${ERASEBLOCK}/g" \
			-e "s/##NBLOCKS##/${NBLOCKS}/g" \
			${CONFIG_FILE}
	fi
}
