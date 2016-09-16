#!/bin/sh
#===============================================================================
#
#  trustfence_sign_uimage.sh
#
#  Copyright (C) 2016 by Digi International Inc.
#  All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License version 2 as published by
#  the Free Software Foundation.
#
#
#  Description:
#    Script for building signed and encrypted kernel uImages using NXP CST.
#
#    The following environment variables define the script behaviour:
#      CONFIG_SIGN_KEYS_PATH: (mandatory) path to the CST folder by NXP with keys generated.
#      CONFIG_KEY_INDEX: (optional) key index to use for signing. Default is 0.
#      CONFIG_DEK_PATH: (optional) Path to keyfile. Define it to generate
#			encrypted images
#
#===============================================================================

SCRIPT_NAME="$(basename ${0})"
SCRIPT_PATH="$(cd $(dirname ${0}) && pwd)"

while getopts "dilp:" c; do
	case "${c}" in
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
    -d               sign/encrypt initramfs
    -i               sign/encrypt DTB
    -l               sign/encrypt Linux image

Supported platforms: ccimx6, ccimx6ul

EOF
}

if [ "${#}" != "2" ]; then
	usage
	exit 1
fi

# Negative offset with respect to CONFIG_RAM_START in which U-Boot
# copies the DEK blob.
DEK_BLOB_OFFSET="0x100"
CONFIG_CSF_SIZE="0x4000"

UIMAGE_PATH="$(readlink -e ${1})"
TARGET="$(readlink -m ${2})"

# Read user configuration file (if used)
[ -f .config ] && . ./.config

if [ -z "${CONFIG_SIGN_KEYS_PATH}" ]; then
	echo "Undefined CONFIG_SIGN_KEYS_PATH";
	exit 1
fi
[ -d "${CONFIG_SIGN_KEYS_PATH}" ] || mkdir "${CONFIG_SIGN_KEYS_PATH}"

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

if [ "${PLATFORM}" = "ccimx6" ]; then
	CONFIG_FDT_LOADADDR="0x18000000"
	CONFIG_RAMDISK_LOADADDR="0x19000000"
	CONFIG_KERNEL_LOADADDR="0x12000000"
elif [ "${PLATFORM}" = "ccimx6ul" ]; then
	CONFIG_FDT_LOADADDR="0x83000000"
	CONFIG_RAMDISK_LOADADDR="0x83800000"
	CONFIG_KERNEL_LOADADDR="0x80800000"
else
	echo "Invalid platform: ${PLATFORM}"
	echo "Supported platforms: ccimx6, ccimx6ul"
	exit 1
fi

[ "${ARTIFACT_DTB}" = "y" ] && CONFIG_RAM_START="${CONFIG_FDT_LOADADDR}"
[ "${ARTIFACT_INITRAMFS}" = "y" ] && CONFIG_RAM_START="${CONFIG_RAMDISK_LOADADDR}"
[ "${ARTIFACT_KERNEL}" = "y" ] && CONFIG_RAM_START="${CONFIG_KERNEL_LOADADDR}"

if [ -z "${CONFIG_RAM_START}" ]; then
        echo "Specify the type of image to process (-i, -d, or -l)"
        exit 1
fi

# Default values
[ -z "${CONFIG_KEY_INDEX}" ] && CONFIG_KEY_INDEX="0"
CONFIG_KEY_INDEX_1="$((CONFIG_KEY_INDEX + 1))"

SRK_KEYS="$(echo ${CONFIG_SIGN_KEYS_PATH}/crts/SRK*crt.pem | sed s/\ /\,/g)"
CERT_CSF="$(echo ${CONFIG_SIGN_KEYS_PATH}/crts/CSF${CONFIG_KEY_INDEX_1}*crt.pem)"
CERT_IMG="$(echo ${CONFIG_SIGN_KEYS_PATH}/crts/IMG${CONFIG_KEY_INDEX_1}*crt.pem)"

n_commas="$(echo ${SRK_KEYS} | grep -o "," | wc -l)"

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

SRK_TABLE="$(pwd)/SRK_table.bin"

# Other constants
GAP_FILLER="0x00"

# The DEK blob is placed by U-Boot just before the kernel image
dek_blob_offset="$((CONFIG_KERNEL_LOADADDR - DEK_BLOB_OFFSET))"

# Compute the layout: sizes and offsets.
uimage_size="$(stat -L -c %s ${UIMAGE_PATH})"
uimage_offset="0x0"
pad_len="$((uimage_size - uimage_size % 0x1000 + 0x1000))"
auth_len="$((pad_len + 0x20))"
sig_len="$((auth_len + CONFIG_CSF_SIZE))"

ivt_uimage_start="$((auth_len - 0x20))"
ivt_ram_start="$((CONFIG_RAM_START + ivt_uimage_start))"
ivt_size="0x20"
csf_ram_start="$((ivt_ram_start + ivt_size))"
entrypoint_uimage_offset="0x1000"
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
	"${SCRIPT_PATH}/csf_templates/encrypt_uimage" > csf_descriptor
else
	sed -e "s,%ram_start%,${CONFIG_RAM_START},g" \
	    -e "s,%srk_table%,${SRK_TABLE},g"		   \
	    -e "s,%image_offset%,${uimage_offset},g"	   \
	    -e "s,%auth_len%,${auth_len},g"		   \
	    -e "s,%cert_csf%,${CERT_CSF},g"		   \
	    -e "s,%cert_img%,${CERT_IMG},g"		   \
	    -e "s,%uimage_path%,${TARGET},g"		   \
	    -e "s,%key_index%,${CONFIG_KEY_INDEX},g"	   \
	"${SCRIPT_PATH}/csf_templates/sign_uimage" > csf_descriptor
fi

# Generate SRK tables
srktool --hab_ver 4 --certs "${SRK_KEYS}" --table "${SRK_TABLE}" --efuses /dev/null --digest sha256
if [ $? -ne 0 ]; then
	echo "[ERROR] Could not generate SRK tables"
	exit 1
fi

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

[ "${ENCRYPT}" = "true" ] && ENCRYPTED_MSG="and encrypted "
echo "Signed ${ENCRYPTED_MSG}image ready: ${TARGET}"
rm -f "${SRK_TABLE}" csf_descriptor csf.bin 2> /dev/null
