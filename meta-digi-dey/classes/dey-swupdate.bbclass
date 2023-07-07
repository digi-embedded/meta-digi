# Copyright (C) 2023 Digi International.
#
# Generates a 'tar.gz' file with the files and folders to be included in the update package
# as part of discrete files SWUpdate installation process.
# 
# Usage:
#
#      In your "local.conf" file, fill the "SWUPDATE_FILES_LIST" variable with the list of
#      files/folders to include in the SWUpdate package. Paths must be relative to "/":
#
#      SWUPDATE_FILES_LIST = "<folder_path> <file_path> ..."
#

# Load commmon variables.
inherit dey-swupdate-common

DEPENDS += "${@oe.utils.ifelse(d.getVar('SWUPDATE_IS_RDIFF_UPDATE') == 'true', 'librsync-native', '')}"

#######################################
###### SWU Update based on files ######
#######################################

create_swupdate_targz_file() {
	local targzfile="${DEPLOY_DIR_IMAGE}/${SWUPDATE_FILES_TARGZ_FILE_NAME}"
	# Clean previous versions of the file.
	rm -f "${targzfile}"

	# Create the tar file including the 'sw-versions' file, as it is mandatory.
	if [ "${SWUPDATE_FILES_TARGZ_FILE}" != "" ]; then
		# User provides a custom tar.gz file. Copy it to distribution dir.
		cp "${SWUPDATE_FILES_TARGZ_FILE}" "${targzfile}"
		# Uncompress the tar file.
		if ! gzip -t "${targzfile}"; then
			# File is not correctly compressed, exit with error.
			echo "[ERROR] File ${SWUPDATE_FILES_TARGZ_FILE} is not a valid 'tar.gz' file. Aborting..."
			exit 1
		fi
		gunzip "${targzfile}"
		# Add the 'sw-versions' file.
		tar -C "${IMAGE_ROOTFS}" -uf "${targzfile%.*}" etc/sw-versions
	else
		# The tar.gz file is not provided by user. Create it including the 'sw-versions' file
		tar -C "${IMAGE_ROOTFS}" -cf "${targzfile%.*}" etc/sw-versions
	fi

	# Iterate the list of files and folders. Add all entries directly except paths starting
	# with 'mnt/linux'. Those files must be added from the 'DEPLOY_DIR_IMAGE' instead of
	# 'IMAGE_ROOTFS', as they are part of the 'boot' image.
	for file in ${SWUPDATE_FILES_LIST}; do
		case "${file}" in
			mnt/linux/*)
				FILE_NAME="$(basename "${file}")"
				tar -C "${DEPLOY_DIR_IMAGE}" --transform 's,^,mnt/linux/,' -uhf "${targzfile%.*}" "${FILE_NAME}"
				;;
			*)
				tar -C "${IMAGE_ROOTFS}" -uf "${targzfile%.*}" "${file}"
				;;
		esac
	done

	# Compress the tar file.
	gzip "${targzfile%.*}"
}
ROOTFS_POSTPROCESS_COMMAND:append = "${@oe.utils.conditional('SWUPDATE_IS_FILES_UPDATE', 'true', ' create_swupdate_targz_file;', '', d)}"

#######################################
###### SWU Update based on RDIFF ######
#######################################

create_swupdate_rdiff_file() {
	local signature_file="${DEPLOY_DIR_IMAGE}/swupdate_rootfs_rdiff.sig"
	local rootfs_rdiff_file="${DEPLOY_DIR_IMAGE}/${SWUPDATE_RDIFF_ROOTFS_DELTA_FILE_NAME}"
	local rootfs_file="${IMGDEPLOYDIR}/${IMAGE_LINK_NAME}.squashfs"

	# Clean previous versions of the files.
	rm -f "${signature_file}" "${rootfs_rdiff_file}"

	# Create signature file.
	rdiff signature "${SWUPDATE_RDIFF_ROOTFS_SOURCE_FILE}" "${signature_file}"

	# Create the delta file.
	rdiff delta "${signature_file}" "${rootfs_file}" "${rootfs_rdiff_file}"

	# Clean intermediates.
	rm -f "${signature_file}"
}
IMAGE_POSTPROCESS_COMMAND:append = "${@oe.utils.conditional('SWUPDATE_IS_RDIFF_UPDATE', 'true', ' create_swupdate_rdiff_file;', '', d)}"
