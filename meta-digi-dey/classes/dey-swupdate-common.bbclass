# Copyright (C) 2023 Digi International.
#

#######################################
########## General variables ##########
#######################################

def get_baseimg_pn(d):
    file_name = d.getVar('PN')
    return file_name[:file_name.find("-swu")] if "-swu" in file_name else file_name

IMAGE_DEPENDS = "${@get_baseimg_pn(d)}"

IMG_NAME = "${IMAGE_DEPENDS}"

# Update description.
SWUPDATE_DESCRIPTION = "${@oe.utils.ifelse(d.getVar('TRUSTFENCE_ENCRYPT_ROOTFS') == '1', 'Encrypted rootfs ${IMG_NAME} update', '${IMG_NAME} update')}"

# Storage type.
SWUPDATE_STORAGE_TYPE = "${@oe.utils.conditional('STORAGE_MEDIA', 'mmc', 'mmc', 'mtd', d)}"

# Root file system type.
SWUPDATE_ROOTFS_TYPE = "${@bb.utils.contains('IMAGE_FEATURES', 'read-only-rootfs', 'squashfs', '', d)}"

# Dual boot partition names for eMMC or MTD
BOOT_DEV_NAME_A ?= "${@bb.utils.contains('STORAGE_MEDIA', 'mmc', '/dev/mmcblk0p1', 'linux_a', d)}"
BOOT_DEV_NAME_B ?= "${@bb.utils.contains('STORAGE_MEDIA', 'mmc', '/dev/mmcblk0p2', 'linux_b', d)}"
ROOTFS_DEV_NAME_A ?= "${@bb.utils.contains('STORAGE_MEDIA', 'mmc', '/dev/mmcblk0p3', 'rootfs_a', d)}"
ROOTFS_DEV_NAME_B ?= "${@bb.utils.contains('STORAGE_MEDIA', 'mmc', '/dev/mmcblk0p4', 'rootfs_b', d)}"

#######################################
###### SWU Update based on files ######
#######################################

# Variable used to generate the tar.gz file. Do not modify.
SWUPDATE_FILES_TARGZ_FILE_NAME = "swupdate-files.tar.gz"

# Initialize variable to provide a custom tar.gz file containing files/dirs to install.
SWUPDATE_FILES_TARGZ_FILE ?= ""

# Initialize variable to store the files/folders that will be part of the SWUpdate package.
SWUPDATE_FILES_LIST ?= ""

# Checks whether SWU update is based on files or not.
def update_based_on_files(d):
    return str(d.getVar('SWUPDATE_FILES_TARGZ_FILE') != "" or d.getVar('SWUPDATE_FILES_LIST') != "").lower()

# Variable that determines if SWU update is based on files or not.
SWUPDATE_IS_FILES_UPDATE = "${@update_based_on_files(d)}"

#######################################
###### SWU Update based on RDIFF ######
#######################################

# Variable used to generate the 'rootfs' RDIFF file. Do not modify.
SWUPDATE_RDIFF_ROOTFS_DELTA_FILE_NAME = "swupdate-rootfs.rdiff"

# Initialize variable to provide the base file from which to generate the 'rootfs' RDIFF file.
SWUPDATE_RDIFF_ROOTFS_SOURCE_FILE ?= ""

# Rdiff image template based on storage type.
SWUPDATE_RDIFF_IMAGE_TEMPLATE_FILE = "${@bb.utils.contains('STORAGE_MEDIA', 'mmc', 'image_template_rdiff_mmc', 'image_template_rdiff_nand', d)}"

# Checks whether SWU update is based on RDIFF or not.
def update_based_on_rdiff(d):
    return str("read-only-rootfs" in d.getVar('IMAGE_FEATURES') and d.getVar('SWUPDATE_IS_FILES_UPDATE') != "true" and d.getVar('SWUPDATE_RDIFF_ROOTFS_SOURCE_FILE') != "").lower()

# Variable that determines if SWU update is based on RDIFF or not.
SWUPDATE_IS_RDIFF_UPDATE = "${@update_based_on_rdiff(d)}"

#######################################
##### SWU Update based on images ######
#######################################

# Image template based on storage type.
SWUPDATE_IMAGES_IMAGE_TEMPLATE_FILE = "${@bb.utils.contains('STORAGE_MEDIA', 'mmc', 'image_template_mmc', 'image_template_nand', d)}"

# Checks whether SWU update is based on images or not.
def update_based_on_images(d):
    return str(d.getVar('SWUPDATE_IS_FILES_UPDATE') != "true" and d.getVar('SWUPDATE_IS_RDIFF_UPDATE') != "true").lower()

# Variable that determines if SWU update is based on images or not.
SWUPDATE_IS_IMAGES_UPDATE = "${@update_based_on_images(d)}"

#######################################
########## SWU Update U-Boot ##########
#######################################

# Determine the correct UBoot update script file to use depending on storage type.
SWUPDATE_UBOOT_SCRIPT = "${@oe.utils.conditional('STORAGE_MEDIA', 'mmc', 'swupdate_uboot_mmc.sh', 'swupdate_uboot_nand.sh', d)}"
SWUPDATE_UBOOT_SCRIPT_NAME = "${@os.path.basename(d.getVar('SWUPDATE_UBOOT_SCRIPT'))}"

SWUPDATE_UBOOT_PREFIX ?= "${UBOOT_PREFIX}"
SWUPDATE_UBOOT_PREFIX:ccmp1 ?= "fip"
SWUPDATE_UBOOT_PREFIX_TFA ?= "tf-a"

SWUPDATE_UBOOT_EXT ?= ".${UBOOT_SUFFIX}"
SWUPDATE_UBOOT_EXT_TFA ?= ".stm32"

SWUPDATE_UBOOT_NAME ?= "${SWUPDATE_UBOOT_PREFIX}-${MACHINE}${SWUPDATE_UBOOT_EXT}"
SWUPDATE_UBOOT_NAME:ccmp1 ?= "${SWUPDATE_UBOOT_PREFIX}-${MACHINE}-optee${FIP_SIGN_SUFFIX}${SWUPDATE_UBOOT_EXT}"
SWUPDATE_UBOOT_NAME_TFA ?= ""
SWUPDATE_UBOOT_NAME_TFA:ccmp1 ?= "${SWUPDATE_UBOOT_PREFIX_TFA}-${MACHINE}-nand${SWUPDATE_UBOOT_EXT_TFA}${TFA_SIGN_SUFFIX}"

SWUPDATE_UBOOT_OFFSET ?= "${BOOTLOADER_SEEK_BOOTPART}"

# Retrieve the correct encryption type.
def get_swupdate_uboot_enc(d):
    if d.getVar('TRUSTFENCE_DEK_PATH') and d.getVar('TRUSTFENCE_DEK_PATH') != "0" :
        return "enc"
    return "normal"

SWUPDATE_UBOOT_ENC ?= "${@get_swupdate_uboot_enc(d)}"

#######################################
########## SWU Update Script ##########
#######################################

# Retrieve the correct update script name based on the SWU update type.
def get_update_script_name(d):
    if d.getVar('SWUPDATE_IS_FILES_UPDATE') == "true":
        return "update_files.sh"
    if d.getVar('SWUPDATE_IS_RDIFF_UPDATE') == "true":
        return "update_rdiff.sh"
    return "update_images.sh"

# Initialize variable that configures the update script to use.
SWUPDATE_SCRIPT ?= "${@get_update_script_name(d)}"

# Name of the update script to include in the SWU package.
SWUPDATE_SCRIPT_NAME = "${@os.path.basename(d.getVar('SWUPDATE_SCRIPT'))}"
