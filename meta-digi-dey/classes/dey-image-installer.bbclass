#
# Generate DEY installer ZIP package
#
# Copyright 2017, Digi International Inc.
#

DEPENDS += "zip-native"

#
# Filesystem types allowed in the installer ZIP
#
FSTYPES_WHITELIST = " \
    boot.ubifs \
    boot.vfat \
    ext4 \
    recovery.ubifs \
    recovery.vfat \
    ubifs \
"

generate_installer_zip () {
	# Get list of files to pack
	INSTALLER_FILELIST="${DEPLOY_DIR_IMAGE}/install_linux_fw_sd.scr"
	for ext in ${FSTYPES_WHITELIST}; do
		if readlink -e "${DEPLOY_DIR_IMAGE}/${IMAGE_LINK_NAME}.${ext}" >/dev/null; then
			INSTALLER_FILELIST="${INSTALLER_FILELIST} ${DEPLOY_DIR_IMAGE}/${IMAGE_LINK_NAME}.${ext}"
		fi
	done
	for ubconf in ${UBOOT_CONFIG}; do
		if readlink -e "${DEPLOY_DIR_IMAGE}/u-boot-${ubconf}.${UBOOT_SUFFIX}" >/dev/null; then
			INSTALLER_FILELIST="${INSTALLER_FILELIST} ${DEPLOY_DIR_IMAGE}/u-boot-${ubconf}.${UBOOT_SUFFIX}"
		fi
	done

	# Create README file
	cat >README.txt <<_EOF_
Digi Embedded Yocto kit installer
---------------------------------

_EOF_
	md5sum ${INSTALLER_FILELIST} | sed -e "s,${DEPLOY_DIR_IMAGE}/,,g" >> README.txt

	# Pack the files
	zip -j ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.installer.zip ${INSTALLER_FILELIST} README.txt

	# Create the symlink
	if [ -n "${IMAGE_LINK_NAME}" ] && [ -e "${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.installer.zip" ]; then
		ln -sf ${IMAGE_NAME}.installer.zip ${DEPLOY_DIR_IMAGE}/${IMAGE_LINK_NAME}.installer.zip
	fi
}

IMAGE_POSTPROCESS_COMMAND += "generate_installer_zip; "
