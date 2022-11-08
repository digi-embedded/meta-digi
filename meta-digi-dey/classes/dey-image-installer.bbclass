#
# Generate DEY installer ZIP package
#
# Copyright 2017-2022, Digi International Inc.
#
inherit boot-artifacts

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

FSTYPES_WHITELIST:ccmp1 = " \
    boot.ubifs \
    recovery.ubifs \
    ubifs \
"

generate_installer_zip () {
	# Get list of files to pack
	INSTALLER_FILELIST="${DEPLOY_DIR_IMAGE}/install_linux_fw_sd.scr \
			    ${DEPLOY_DIR_IMAGE}/install_linux_fw_usb.scr"
	# Get UUU installation script
	if readlink -e "${DEPLOY_DIR_IMAGE}/install_linux_fw_uuu.sh"; then
		INSTALLER_FILELIST="${INSTALLER_FILELIST} ${DEPLOY_DIR_IMAGE}/install_linux_fw_uuu.sh"
	fi
	# Decompress the ext4.gz image, if any
	if readlink -e "${IMGDEPLOYDIR}/${IMAGE_LINK_NAME}.ext4.gz" >/dev/null; then
		gzip -d -k -f ${IMGDEPLOYDIR}/${IMAGE_LINK_NAME}.ext4.gz
	fi
	for ext in ${FSTYPES_WHITELIST}; do
		if readlink -e "${IMGDEPLOYDIR}/${IMAGE_LINK_NAME}.${ext}" >/dev/null; then
			INSTALLER_FILELIST="${INSTALLER_FILELIST} ${IMGDEPLOYDIR}/${IMAGE_LINK_NAME}.${ext}"
		fi
	done
	for artifact in ${BOOTABLE_ARTIFACTS}; do
		if readlink -e "${DEPLOY_DIR_IMAGE}/${artifact}" >/dev/null; then
			INSTALLER_FILELIST="${INSTALLER_FILELIST} ${DEPLOY_DIR_IMAGE}/${artifact}"
		fi
	done

	# Create README file
	cat >${IMGDEPLOYDIR}/README.txt <<_EOF_
${DISTRO_NAME} ${DISTRO_VERSION} kit installer
----------------------------------------

_EOF_
	md5sum ${INSTALLER_FILELIST} | sed -e "s,${DEPLOY_DIR_IMAGE}/,,g;s,${IMGDEPLOYDIR}/,,g" >> ${IMGDEPLOYDIR}/README.txt

	# Pack the files and remove the temporary readme file
	zip -j ${IMGDEPLOYDIR}/${IMAGE_NAME}.installer.zip ${INSTALLER_FILELIST} ${IMGDEPLOYDIR}/README.txt
	rm -f ${IMGDEPLOYDIR}/README.txt

	# Delete the decompressed ext4 image, if any
	if readlink -e "${IMGDEPLOYDIR}/${IMAGE_LINK_NAME}.ext4" >/dev/null; then
		rm -f ${IMGDEPLOYDIR}/${IMAGE_LINK_NAME}.ext4
	fi

	# Create the symlink
	if [ -n "${IMAGE_LINK_NAME}" ] && [ -e "${IMGDEPLOYDIR}/${IMAGE_NAME}.installer.zip" ]; then
		ln -sf ${IMAGE_NAME}.installer.zip ${IMGDEPLOYDIR}/${IMAGE_LINK_NAME}.installer.zip
	fi
}

generate_installer_zip:append:ccimx8m () {
	INSTALLER_FILELIST=${META_DIGI_SCRIPTS}/install_usb_driver.sh
}

generate_installer_zip:append:ccmp1 () {
	INSTALLER_FILELIST=${META_DIGI_SCRIPTS}/install_usb_driver.sh
}

IMAGE_POSTPROCESS_COMMAND += "generate_installer_zip; "
