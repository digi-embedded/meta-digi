# Copyright (C) 2021-2024, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

# Chose a between a default hard-coded config file (for read-only rootfs)
# or a dynamically generated one (with a postinst script)
FW_CONFIG_FILE = "${@bb.utils.contains('STORAGE_MEDIA', 'mtd', \
			bb.utils.contains('IMAGE_FEATURES', 'read-only-rootfs', \
				'${STORAGE_MEDIA}/fw_env.config_default', \
				'${STORAGE_MEDIA}/fw_env.config', d), \
			'${STORAGE_MEDIA}/fw_env.config', \
			 d)}"

FW_CONFIG_FILE:ccmp1 = "${@bb.utils.contains('IMAGE_FEATURES', 'read-only-rootfs', \
				'ubi/fw_env.config_default', \
				'ubi/fw_env.config', d)}"

DEPENDS += "${@oe.utils.conditional('OPTEE_PATCHES', '', '', 'optee-client', d)}"

OPTEE_PATCHES = ""
OPTEE_PATCHES:ccimx9 = "file://0004-Implement-support-for-environment-encryption-using-O.patch"
OPTEE_PATCHES:ccmp1 = "file://0004-Implement-support-for-environment-encryption-using-O.patch"

SRC_URI += " \
    file://${FW_CONFIG_FILE} \
    file://0001-Implement-U-Boot-environment-access-functions.patch \
    file://0002-tools-env-add-support-to-set-dynamic-location-of-env.patch \
    file://0003-Implement-support-for-environment-encryption-by-CAAM.patch \
    ${@bb.utils.contains('MACHINE_FEATURES', 'optee', '${OPTEE_PATCHES}', '', d)} \
"

do_install:append() {
	install -d ${D}${sysconfdir}
	install -m 0644 ${WORKDIR}/${FW_CONFIG_FILE} ${D}${sysconfdir}/fw_env.config
}

UBOOT_ENV_PARTITION = "environment"
UBOOT_ENV_PARTITION:ccmp1 = "UBI"

pkg_postinst_ontarget:${PN}() {
	CONFIG_FILE="/etc/fw_env.config"
	MMCDEV="$(sed -ne 's,.*root=/dev/mmcblk\([0-9]\)p.*,\1,g;T;p' /proc/cmdline)"
	if [ -n "${MMCDEV}" ]; then
		sed -i -e "s,^/dev/mmcblk[^[:blank:]]\+,/dev/mmcblk${MMCDEV},g" ${CONFIG_FILE}
	fi

	PARTTABLE="/proc/mtd"
	MTDINDEX="$(sed -ne "s/\(^mtd[0-9]\+\):.*\<${UBOOT_ENV_PARTITION}\>.*/\1/g;T;p" ${PARTTABLE} 2>/dev/null)"
	if [ -n "${MTDINDEX}" ]; then
		# Initialize variables for fixed offset values
		# (backwards compatible with old U-Boot)
		ENV_OFFSET="${UBOOT_ENV_OFFSET}"
		ENV_REDUND_OFFSET="${UBOOT_ENV_REDUND_OFFSET}"
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

inherit ${@bb.utils.contains("IMAGE_FEATURES", "read-only-rootfs", "remove-pkg-postinst-ontarget", "", d)}
