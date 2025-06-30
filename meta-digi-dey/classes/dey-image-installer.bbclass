#
# Generate DEY installer ZIP package
#
# Copyright 2017-2024, Digi International Inc.
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
    squashfs \
"

FSTYPES_WHITELIST:ccmp1 = " \
    boot.ubifs \
    recovery.ubifs \
    ubifs \
    squashfs \
"

HAS_USB_DRIVER = "false"
HAS_USB_DRIVER:ccimx8m = "true"
HAS_USB_DRIVER:ccimx9 = "true"
HAS_USB_DRIVER:ccmp1 = "true"
HAS_USB_DRIVER:ccmp2 = "true"

BOOTLOADER_SIGNED_STRING ?= "-signed"
BOOTLOADER_ENCRYPTED_STRING ?= "-encrypted"
BOOTLOADER_SIGNED_USB_STRING ?= "-usb-signed"

curate_bootloader_artifacts() {
	for artifact in ${BOOTABLE_ARTIFACTS}; do
		# NXP platforms may have a ##SIGNED## placeholder to replace
		if [ "${DEY_SOC_VENDOR}" = "NXP" ] && echo "${artifact}" | grep -q -e "##SIGNED##"; then
			if [ "${TRUSTFENCE_SIGN}" = "1" ]; then
				if [ "${DIGI_SOM}" = "ccimx6ul" ]; then
					if [ "${TRUSTFENCE_DEK_PATH}" != "0" ]; then
						# Encrypted bootloader
						curated_artifact=$(echo "${artifact}" | sed "s,##SIGNED##,${BOOTLOADER_ENCRYPTED_STRING},")
						CURATED_BOOTABLE_ARTIFACTS="${CURATED_BOOTABLE_ARTIFACTS} ${curated_artifact}"
					else
						# Signed, non-encrypted bootloader
						curated_artifact=$(echo "${artifact}" | sed "s,##SIGNED##,${BOOTLOADER_SIGNED_STRING},")
						CURATED_BOOTABLE_ARTIFACTS="${CURATED_BOOTABLE_ARTIFACTS} ${curated_artifact}"
					fi
					# Signed, non-encrypted bootloader for USB recovery
					curated_artifact=$(echo "${artifact}" | sed "s,##SIGNED##,${BOOTLOADER_SIGNED_USB_STRING},")
					CURATED_BOOTABLE_ARTIFACTS="${CURATED_BOOTABLE_ARTIFACTS} ${curated_artifact}"
				else
					if [ "${TRUSTFENCE_DEK_PATH}" != "0" ]; then
						# Encrypted bootloader
						curated_artifact=$(echo "${artifact}" | sed "s,##SIGNED##,${BOOTLOADER_ENCRYPTED_STRING},")
						CURATED_BOOTABLE_ARTIFACTS="${CURATED_BOOTABLE_ARTIFACTS} ${curated_artifact}"
					fi
					# Signed, non-encrypted bootloader for USB recovery
					curated_artifact=$(echo "${artifact}" | sed "s,##SIGNED##,${BOOTLOADER_SIGNED_STRING},")
					CURATED_BOOTABLE_ARTIFACTS="${CURATED_BOOTABLE_ARTIFACTS} ${curated_artifact}"
				fi
			else
				# Non-signed bootloader
				curated_artifact=$(echo "${artifact}" | sed 's,##SIGNED##,,')
				CURATED_BOOTABLE_ARTIFACTS="${CURATED_BOOTABLE_ARTIFACTS} ${curated_artifact}"
			fi
		else
			CURATED_BOOTABLE_ARTIFACTS="${CURATED_BOOTABLE_ARTIFACTS} ${artifact}"
		fi
	done
	export CURATED_BOOTABLE_ARTIFACTS="${CURATED_BOOTABLE_ARTIFACTS}"
}

generate_installer_zip () {
	# Get list of files to pack
	INSTALLER_FILELIST="${DEPLOY_DIR_IMAGE}/install_linux_fw_sd.scr \
			    ${DEPLOY_DIR_IMAGE}/install_linux_fw_usb.scr"
	# Get UUU installation script
	if readlink -e "${DEPLOY_DIR_IMAGE}/install_linux_fw_uuu.sh"; then
		INSTALLER_FILELIST="${INSTALLER_FILELIST} ${DEPLOY_DIR_IMAGE}/install_linux_fw_uuu.sh"
	fi
	# Get USB driver installation script
	if ${HAS_USB_DRIVER}; then
		INSTALLER_FILELIST="${INSTALLER_FILELIST} ${META_DIGI_SCRIPTS}/install_usb_driver.sh"
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

	# Add bootable artifacts to installer
	curate_bootloader_artifacts
	for artifact in ${CURATED_BOOTABLE_ARTIFACTS}; do
		if readlink -e "${DEPLOY_DIR_IMAGE}/${artifact}" >/dev/null; then
			INSTALLER_FILELIST="${INSTALLER_FILELIST} ${DEPLOY_DIR_IMAGE}/${artifact}"
		fi
	done

	# Create README file
	cat >${IMGDEPLOYDIR}/README.txt <<_EOF_
${DISTRO_NAME} ${DISTRO_VERSION} kit installer
----------------------------------------

_EOF_
	md5sum ${INSTALLER_FILELIST} | sed -e "s,${DEPLOY_DIR_IMAGE}.*/,,g;s,${IMGDEPLOYDIR}/,,g;s,${META_DIGI_SCRIPTS}/,,g" >> ${IMGDEPLOYDIR}/README.txt

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

IMAGE_POSTPROCESS_COMMAND += "generate_installer_zip "
