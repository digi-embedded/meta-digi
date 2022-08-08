# Adds DualBoot configuration
#
# To use it add the following line to conf/local.conf:
#
# INHERIT += "dualboot"
#

DUALBOOT_ENABLED ?= "true"

# U-Boot altboot script
BOOT_SCRIPTS += "altboot.scr:altboot.scr"

# Dual boot partition names for eMMC or MTD
BOOT_DEV_NAME_A ?= "${@bb.utils.contains('STORAGE_MEDIA', 'mmc', '/dev/mmcblk0p1', 'linux_a', d)}"
BOOT_DEV_NAME_B ?= "${@bb.utils.contains('STORAGE_MEDIA', 'mmc', '/dev/mmcblk0p2', 'linux_b', d)}"
ROOTFS_DEV_NAME_A ?= "${@bb.utils.contains('STORAGE_MEDIA', 'mmc', '/dev/mmcblk0p3', 'rootfs_a', d)}"
ROOTFS_DEV_NAME_B ?= "${@bb.utils.contains('STORAGE_MEDIA', 'mmc', '/dev/mmcblk0p4', 'rootfs_b', d)}"

fill_description_dualboot () {
	sed -i -e "s,##BOOTIMG_NAME##,${IMG_NAME}-${MACHINE}${BOOTFS_EXT},g" "${WORKDIR}/sw-description-dualboot"
	sed -i -e "s,##BOOT_DEV_A##,${BOOT_DEV_NAME_A},g" "${WORKDIR}/sw-description-dualboot"
	sed -i -e "s,##BOOT_DEV_B##,${BOOT_DEV_NAME_B},g" "${WORKDIR}/sw-description-dualboot"
	sed -i -e "s,##ROOTIMG_NAME##,${IMG_NAME}-${MACHINE}${ROOTFS_EXT},g" "${WORKDIR}/sw-description-dualboot"
	sed -i -e "s,##ROOTFS_DEV_A##,${ROOTFS_DEV_NAME_A},g" "${WORKDIR}/sw-description-dualboot"
	sed -i -e "s,##ROOTFS_DEV_B##,${ROOTFS_DEV_NAME_B},g" "${WORKDIR}/sw-description-dualboot"
	sed -i -e "s,##SW_VERSION##,${SOFTWARE_VERSION},g" "${WORKDIR}/sw-description-dualboot"
	# Overwrite the sw-description with the dualboot version
	mv ${WORKDIR}/sw-description-dualboot ${WORKDIR}/sw-description
}