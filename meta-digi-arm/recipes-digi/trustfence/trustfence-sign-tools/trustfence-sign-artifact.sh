#!/bin/sh
#===============================================================================
#
#  trustfence-sign-artifact.sh
#
#  Copyright (C) 2016-2020 by Digi International Inc.
#  All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License version 2 as published by
#  the Free Software Foundation.
#
#
#  Description:
#    Script for building signed and encrypted artifacts using NXP CST.
#
#    The following environment variables define the script behaviour:
#      CONFIG_SIGN_KEYS_PATH: (mandatory) path to the CST folder by NXP with keys generated.
#      CONFIG_SIGN_MODE: (mandatory) Signing method: HAB/AHAB
#      CONFIG_KEY_INDEX: (optional) key index to use for signing. Default is 0.
#      CONFIG_DEK_PATH: (optional) Path to keyfile. Define it to generate
#			encrypted images
#
#===============================================================================

# Avoid parallel execution of this script
SINGLE_PROCESS_LOCK="/tmp/sign_script.lock.d"
trap 'rm -rf "${SINGLE_PROCESS_LOCK}"' INT TERM EXIT
while ! mkdir "${SINGLE_PROCESS_LOCK}" > /dev/null 2>&1; do
        sleep 1
done

SCRIPT_NAME="$(basename ${0})"
SCRIPT_PATH="$(cd $(dirname ${0}) && pwd)"

while getopts "bdilp:" c; do
	case "${c}" in
		b) ARTIFACT_BOOTSCRIPT="y";;
		d) ARTIFACT_DTB="y";;
		i) ARTIFACT_INITRAMFS="y";;
		l) ARTIFACT_KERNEL="y";;
		p) PLATFORM="${OPTARG}";;
	esac
done
shift "$((OPTIND - 1))"

usage() {
        cat <<EOF

Usage: ${SCRIPT_NAME} [OPTIONS] input-unsigned-image output-signed-image

    -p <platform>    select platform for the project
    -b               sign/encrypt bootscript
    -d               sign/encrypt DTB
    -i               sign/encrypt initramfs
    -l               sign/encrypt Linux image

Supported platforms: ccimx6, ccimx6ul, ccimx8x, ccimx8mn

EOF
}

if [ "${#}" != "2" ]; then
	usage
	exit 1
fi

UIMAGE_PATH="$(readlink -e ${1})"
TARGET="$(readlink -m ${2})"

# Read user configuration file (if used)
[ -f .config ] && . ./.config

if [ -z "${CONFIG_SIGN_KEYS_PATH}" ]; then
	echo "Undefined CONFIG_SIGN_KEYS_PATH";
	exit 1
fi
[ -d "${CONFIG_SIGN_KEYS_PATH}" ] || mkdir "${CONFIG_SIGN_KEYS_PATH}"
if [ -z "${CONFIG_SIGN_MODE}" ]; then
	echo "Undefined CONFIG_SIGN_MODE";
	exit 1
fi

# Get RAM_START address
if [ "${PLATFORM}" = "ccimx6" ]; then
	CONFIG_FDT_LOADADDR="0x18000000"
	CONFIG_RAMDISK_LOADADDR="0x19000000"
	CONFIG_KERNEL_LOADADDR="0x12000000"
	CONFIG_CSF_SIZE="0x4000"
elif [ "${PLATFORM}" = "ccimx6ul" ]; then
	CONFIG_FDT_LOADADDR="0x83000000"
	CONFIG_RAMDISK_LOADADDR="0x83800000"
	CONFIG_KERNEL_LOADADDR="0x80800000"
	CONFIG_CSF_SIZE="0x4000"
elif [ "${PLATFORM}" = "ccimx8x" ]; then
	CONFIG_FDT_LOADADDR="0x82000000"
	CONFIG_RAMDISK_LOADADDR="0x82100000"
	CONFIG_KERNEL_LOADADDR="0x80280000"
elif [ "${PLATFORM}" = "ccimx8mn" ]; then
	CONFIG_FDT_LOADADDR="0x43000000"
	CONFIG_RAMDISK_LOADADDR="0x43800000"
	CONFIG_KERNEL_LOADADDR="0x40480000"
	CONFIG_CSF_SIZE="0x2000"
else
	echo "Invalid platform: ${PLATFORM}"
	echo "Supported platforms: ccimx6, ccimx6ul, ccimx8x, ccimx8mn"
	exit 1
fi

[ "${ARTIFACT_DTB}" = "y" ] && CONFIG_RAM_START="${CONFIG_FDT_LOADADDR}"
[ "${ARTIFACT_INITRAMFS}" = "y" ] && CONFIG_RAM_START="${CONFIG_RAMDISK_LOADADDR}"
[ "${ARTIFACT_KERNEL}" = "y" ] && CONFIG_RAM_START="${CONFIG_KERNEL_LOADADDR}"
# bootscripts are loaded to $loadaddr, just like the kernel
[ "${ARTIFACT_BOOTSCRIPT}" = "y" ] && CONFIG_RAM_START="${CONFIG_KERNEL_LOADADDR}"

if [ -z "${CONFIG_RAM_START}" ]; then
	echo "Specify the type of image to process (-b, -i, -d, or -l)"
	exit 1
fi

if [ "${CONFIG_SIGN_MODE}" = "HAB" ]; then
	# Negative offset with respect to CONFIG_RAM_START in which U-Boot
	# copies the DEK blob.
	DEK_BLOB_OFFSET="0x100"
	if [ -n "${CONFIG_DEK_PATH}" ]; then
		if [ ! -f "${CONFIG_DEK_PATH}" ]; then
			echo "DEK not found. Generating random 256 bit DEK."
			[ -d $(dirname ${CONFIG_DEK_PATH}) ] || mkdir -p $(dirname ${CONFIG_DEK_PATH})
			dd if=/dev/urandom of="${CONFIG_DEK_PATH}" bs=32 count=1 >/dev/null 2>&1
		fi
		dek_size="$((8 * $(stat -L -c %s ${CONFIG_DEK_PATH})))"
		if [ "${dek_size}" != "128" ] && [ "${dek_size}" != "192" ] && [ "${dek_size}" != "256" ]; then
			echo "Invalid DEK size: ${dek_size} bits. Valid sizes are 128, 192 and 256 bits"
			exit 1
		fi
		ENCRYPT="true"
	fi
fi

# Default values
[ -z "${CONFIG_KEY_INDEX}" ] && CONFIG_KEY_INDEX="0"
CONFIG_KEY_INDEX_1="$((CONFIG_KEY_INDEX + 1))"

SRK_KEYS="$(echo ${CONFIG_SIGN_KEYS_PATH}/crts/SRK*crt.pem | sed s/\ /\,/g)"
if [ "${CONFIG_SIGN_MODE}" = "HAB" ]; then
	CERT_CSF="$(echo ${CONFIG_SIGN_KEYS_PATH}/crts/CSF${CONFIG_KEY_INDEX_1}*crt.pem)"
	CERT_IMG="$(echo ${CONFIG_SIGN_KEYS_PATH}/crts/IMG${CONFIG_KEY_INDEX_1}*crt.pem)"
fi

n_commas="$(echo ${SRK_KEYS} | grep -o "," | wc -l)"

if [ "${CONFIG_SIGN_MODE}" = "HAB" ]; then
	if [ "${n_commas}" -eq 3 ] && [ -f "${CERT_CSF}" ] && [ -f "${CERT_IMG}" ]; then
		# PKI tree already exists.
		echo "Using existing PKI tree"
	elif [ "${n_commas}" -eq 0 ] || [ ! -f "${CERT_CSF}" ] || [ ! -f "${CERT_IMG}" ]; then
		# Generate PKI
		trustfence-gen-pki.sh "${CONFIG_SIGN_KEYS_PATH}"

		SRK_KEYS="$(echo ${CONFIG_SIGN_KEYS_PATH}/crts/SRK*crt.pem | sed s/\ /\,/g)"
		CERT_CSF="$(echo ${CONFIG_SIGN_KEYS_PATH}/crts/CSF${CONFIG_KEY_INDEX_1}*crt.pem)"
		CERT_IMG="$(echo ${CONFIG_SIGN_KEYS_PATH}/crts/IMG${CONFIG_KEY_INDEX_1}*crt.pem)"
	else
		echo "Inconsistent CST folder."
		exit 1
	fi
elif [ "${CONFIG_SIGN_MODE}" = "AHAB" ]; then
	if [ "${n_commas}" -eq 3 ] && [ "${CONFIG_SIGN_MODE}" = "AHAB" ]; then
		# PKI tree already exists. Do nothing
		echo "Using existing PKI tree"
	elif [ "${n_commas}" -eq 0 ] && [ "${CONFIG_SIGN_MODE}" = "AHAB" ]; then
		# Generate PKI
		trustfence-gen-pki.sh "${CONFIG_SIGN_KEYS_PATH}"

		SRK_KEYS="$(echo ${CONFIG_SIGN_KEYS_PATH}/crts/SRK*crt.pem | sed s/\ /\,/g)"
	else
		echo "Inconsistent CST folder."
		exit 1
	fi
fi

LINUX64_MAGIC="0x644d5241"

get_image_size()
{
	# Check if LINUX_ARM64 image magic number is found
	magic_number="$(hexdump -n 4 -s 56 -e '/4 "0x%08x\t" "\n"' ${UIMAGE_PATH})"
	if [ ${magic_number} = "${LINUX64_MAGIC}" ]; then
		# LINUX_ARM64, read the size from the file header
		image_size="$(hexdump -n 4 -s 16 -e '/4 "0x%08x\t" "\n"' ${UIMAGE_PATH})"
	else
		# Unknown image type, return the actual filesize
		image_size="$(stat -L -c %s ${UIMAGE_PATH})"
	fi
	echo ${image_size}
}

SRK_TABLE="$(pwd)/SRK_table.bin"
if [ "${CONFIG_SIGN_MODE}" = "HAB" ]; then
	HAB_VER="hab_ver 4"
	DIGEST="digest"
	DIGEST_ALGO="sha256"

	# Other constants
	GAP_FILLER="0x00"

	# The DEK blob is placed by U-Boot just before the kernel image
	dek_blob_offset="$((CONFIG_KERNEL_LOADADDR - DEK_BLOB_OFFSET))"

	# Compute the layout: sizes and offsets.
	uimage_size="$(get_image_size)"
	uimage_offset="0x0"
	pad_len="$(((uimage_size + 0x1000 - 1) & ~(0x1000 - 1)))"
	auth_len="$((pad_len + 0x20))"
	sig_len="$((auth_len + CONFIG_CSF_SIZE))"

	ivt_uimage_start="$((auth_len - 0x20))"
	ivt_ram_start="$((CONFIG_RAM_START + ivt_uimage_start))"
	ivt_size="0x20"
	csf_ram_start="$((ivt_ram_start + ivt_size))"
	entrypoint_uimage_offset="0x100"
	entrypoint_ram_start="$((CONFIG_RAM_START + entrypoint_uimage_offset))"
	entrypoint_size="0x20"
	header_uimage_offset="0x0"
	header_ram_start="${CONFIG_RAM_START}"
	header_size="0x40"

	r1_uimage_offset="${header_size}"
	r1_ram_start="$((CONFIG_RAM_START + r1_uimage_offset))"
	r1_size="$((entrypoint_uimage_offset - header_size ))"
	r2_uimage_offset="$((entrypoint_uimage_offset + entrypoint_size))"
	r2_ram_start="$((CONFIG_RAM_START + r2_uimage_offset))"
	r2_size="$((ivt_uimage_start - (entrypoint_uimage_offset + entrypoint_size)))"

	# Generate actual CSF descriptor file from template
	if [ "${ENCRYPT}" = "true" ]; then
		sed -e "s,%ram_start%,${CONFIG_RAM_START},g"		    \
		-e "s,%srk_table%,${SRK_TABLE},g "				    \
		-e "s,%cert_csf%,${CERT_CSF},g"				    \
		-e "s,%cert_img%,${CERT_IMG},g"				    \
		-e "s,%uimage_path%,${TARGET},g"				    \
		-e "s,%key_index%,${CONFIG_KEY_INDEX},g"			    \
		-e "s,%dek_len%,${dek_size},g"				    \
		-e "s,%dek_path%,${CONFIG_DEK_PATH},g"			    \
		-e "s,%dek_offset%,${dek_blob_offset},g"			    \
		-e "s,%ivt_uimage_start%,${ivt_uimage_start},g"		    \
		-e "s,%ivt_ram_start%,${ivt_ram_start},g"			    \
		-e "s,%ivt_size%,${ivt_size},g"				    \
		-e "s,%entrypoint_uimage_offset%,${entrypoint_uimage_offset},g" \
		-e "s,%entrypoint_ram_start%,${entrypoint_ram_start},g"	    \
		-e "s,%entrypoint_size%,${entrypoint_size},g"		    \
		-e "s,%header_uimage_offset%,${header_uimage_offset},g"	    \
		-e "s,%header_ram_start%,${header_ram_start},g"		    \
		-e "s,%header_size%,${header_size},g"			    \
		-e "s,%r1_uimage_offset%,${r1_uimage_offset},g"		    \
		-e "s,%r1_ram_start%,${r1_ram_start},g"			    \
		-e "s,%r1_size%,${r1_size},g"				    \
		-e "s,%r2_uimage_offset%,${r2_uimage_offset},g"		    \
		-e "s,%r2_ram_start%,${r2_ram_start},g"			    \
		-e "s,%r2_size%,${r2_size},g"				    \
		"${SCRIPT_PATH}/csf_templates/encrypt_hab" > csf_descriptor
	else
		sed -e "s,%ram_start%,${CONFIG_RAM_START},g" \
		-e "s,%srk_table%,${SRK_TABLE},g"		   \
		-e "s,%image_offset%,${uimage_offset},g"	   \
		-e "s,%auth_len%,${auth_len},g"		   \
		-e "s,%cert_csf%,${CERT_CSF},g"		   \
		-e "s,%cert_img%,${CERT_IMG},g"		   \
		-e "s,%uimage_path%,${TARGET},g"		   \
		-e "s,%key_index%,${CONFIG_KEY_INDEX},g"	   \
		"${SCRIPT_PATH}/csf_templates/sign_hab" > csf_descriptor
	fi
else
	# Other constants
	KERNEL_START_OFFSET="0x0"
	KERNEL_SIG_BLOCK_OFFSET="0x90"
	KERNEL_NAME="${1}"

	HAB_VER="ahab"
	DIGEST="sign_digest"
	DIGEST_ALGO="sha512"

	# Compute the layout: sizes and offsets.
	container_header_offset="${KERNEL_START_OFFSET}"
	signature_block_offset="${KERNEL_SIG_BLOCK_OFFSET}"

	SRK_CERT_KEY_IMG="$(echo ${CONFIG_SIGN_KEYS_PATH}/crts/SRK${CONFIG_KEY_INDEX_1}*crt.pem | sed s/\ /\,/g)"

	sed -e "s,%srk_table%,${SRK_TABLE},g"		   \
	-e "s,%cert_img%,${SRK_CERT_KEY_IMG},g"		   \
	-e "s,%kernel-img%,${KERNEL_NAME},g"		   \
	-e "s,%key_index%,${CONFIG_KEY_INDEX},g"	   \
	-e "s,%container_offset%,${container_header_offset},g" \
	-e "s,%block_offset%,${signature_block_offset},g" \
	"${SCRIPT_PATH}/csf_templates/sign_ahab" > csf_descriptor

	if [ "${ENCRYPT}" = "true" ]; then
		echo "[ERROR] Environment encryption is not supported."
		exit 1
	fi
fi

# Generate SRK tables
srktool --${HAB_VER} --certs "${SRK_KEYS}" --table "${SRK_TABLE}" --efuses /dev/null --${DIGEST} "${DIGEST_ALGO}"
if [ $? -ne 0 ]; then
	echo "[ERROR] Could not generate SRK tables"
	exit 1
fi

if [ "${CONFIG_SIGN_MODE}" = "HAB" ]; then
	# Pad to IVT
	objcopy -I binary -O binary --pad-to "${pad_len}" --gap-fill="${GAP_FILLER}" "${UIMAGE_PATH}" "${TARGET}"

	# Generate and attach IVT
	# Fields: header, jump location, reserved (0), DCD pointer (null)
	#	  boot data (null), self pointer, CSF pointer, reserved (0)
	PRINTF="$(which printf)"
	IVT_HEADER="0x402000D1"
	{
		${PRINTF} $(${PRINTF} "%08x" ${IVT_HEADER} | sed 's/.\{2\}/&\n/g' | tac | sed 's,^,\\x,g' | tr -d '\n')
		${PRINTF} $(${PRINTF} "%08x" ${entrypoint_ram_start} | sed 's/.\{2\}/&\n/g' | tac | sed 's,^,\\x,g' | tr -d '\n')
		${PRINTF} $(${PRINTF} "%08x" 0 | sed 's/.\{2\}/&\n/g' | tac | sed 's,^,\\x,g' | tr -d '\n')
		${PRINTF} $(${PRINTF} "%08x" 0 | sed 's/.\{2\}/&\n/g' | tac | sed 's,^,\\x,g' | tr -d '\n')
		${PRINTF} $(${PRINTF} "%08x" 0 | sed 's/.\{2\}/&\n/g' | tac | sed 's,^,\\x,g' | tr -d '\n')
		${PRINTF} $(${PRINTF} "%08x" ${ivt_ram_start} | sed 's/.\{2\}/&\n/g' | tac | sed 's,^,\\x,g' | tr -d '\n')
		${PRINTF} $(${PRINTF} "%08x" ${csf_ram_start} | sed 's/.\{2\}/&\n/g' | tac | sed 's,^,\\x,g' | tr -d '\n')
		${PRINTF} $(${PRINTF} "%08x" 0 | sed 's/.\{2\}/&\n/g' | tac | sed 's,^,\\x,g' | tr -d '\n')
	} >> "${TARGET}"

	CURRENT_PATH="$(pwd)"
	cst -o "${CURRENT_PATH}/csf.bin" -i "${CURRENT_PATH}/csf_descriptor" >/dev/null
	if [ $? -ne 0 ]; then
		echo "[ERROR] Could not generate CSF"
		exit 1
	fi

	cat csf.bin >> "${TARGET}"

	objcopy -I binary -O binary --pad-to "${sig_len}" --gap-fill="${GAP_FILLER}" "${TARGET}"
else
	# Prepare the image container
	mkimage_imx8 -soc "QX" -rev "B0" -c -ap ${UIMAGE_PATH} a35 ${CONFIG_RAM_START} -out temp-mkimg
	mv temp-mkimg "${UIMAGE_PATH}"

	# Sign the image
	CURRENT_PATH="$(pwd)"
	cst -o "${TARGET}" -i "${CURRENT_PATH}/csf_descriptor" >/dev/null
	if [ $? -ne 0 ]; then
		echo "[ERROR] Could not generate CSF $?"
		exit 1
	fi
fi

[ "${ENCRYPT}" = "true" ] && ENCRYPTED_MSG="and encrypted "
echo "Signed ${ENCRYPTED_MSG}image ready: ${TARGET}"
rm -f "${SRK_TABLE}" csf_descriptor csf.bin 2> /dev/null
