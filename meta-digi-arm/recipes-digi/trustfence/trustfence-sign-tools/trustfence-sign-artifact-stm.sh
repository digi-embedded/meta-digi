#!/bin/sh
#===============================================================================
#
#  trustfence-sign-artifact.sh
#
#  Copyright (C) 2023 by Digi International Inc.
#  All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License version 2 as published by
#  the Free Software Foundation.
#
#
#  Description:
#    Script for building signed and encrypted artifacts using STM sign tools.
#
#    The following environment variables define the script behaviour:
#
#===============================================================================

# Avoid parallel execution of this script
SINGLE_PROCESS_LOCK="/tmp/sign_script.lock.d"
trap 'rm -rf "${SINGLE_PROCESS_LOCK}"' INT TERM EXIT
while ! mkdir "${SINGLE_PROCESS_LOCK}" > /dev/null 2>&1; do
	sleep 1
done

SCRIPT_NAME="$(basename "${0}")"
SUPPORTED_PLATFORMS="ccmp15, ccmp13"

while getopts "p:t" c; do
	case "${c}" in
		p) PLATFORM="${OPTARG}";;
		t) ARTIFACT_TFA="y";;
	esac
done
shift "$((OPTIND - 1))"

usage() {
	cat <<EOF

Usage: ${SCRIPT_NAME} <OPTIONS> [<input-unsigned-image> <output-signed-image>]

 Options:
    -p <platform>    platform
    -t               sign/encrypt TF-A artifact

Supported platforms: ${SUPPORTED_PLATFORMS}

EOF
}

if [ "${#}" != "2" ]; then
	usage
	exit 1
fi

if [ -z "${CONFIG_SIGN_KEYS_PATH}" ]; then
	echo "Undefined CONFIG_SIGN_KEYS_PATH";
	exit 1
fi

# Default values
[ -z "${CONFIG_KEY_INDEX}" ] && CONFIG_KEY_INDEX="0"
KEY_PASS_FILE="${CONFIG_SIGN_KEYS_PATH}/keys/key_pass.txt"

# Generate random keys if they don't exist
if ! trustfence-gen-pki.sh -p ${PLATFORM}; then
	exit 1
fi

if [ "${PLATFORM}" = "ccmp15" ]; then
	PUBLIC_KEY="${CONFIG_SIGN_KEYS_PATH}/keys/publicKey00.pem"
	PRIVATE_KEY="${CONFIG_SIGN_KEYS_PATH}/keys/privateKey00.pem"
elif [ "${PLATFORM}" = "ccmp13" ]; then
	PUBLIC_KEY="${CONFIG_SIGN_KEYS_PATH}/keys/publicKey0*.pem"
	PRIVATE_KEY="${CONFIG_SIGN_KEYS_PATH}/keys/privateKey0${CONFIG_KEY_INDEX}.pem"
else
	echo "Undefined platform"
	exit 1
fi

if [ "${ARTIFACT_TFA}" != "y" ]; then
	echo "Specify the type of image to process (-t)"
	usage
	exit 1
fi

INPUT_FILE="$(readlink -e "${1}")"
OUTPUT_FILE="$(readlink -m "${2}")"

# Obtain password from key pass file
INDEX=$((CONFIG_KEY_INDEX + 1))
PASS=$(cat "${KEY_PASS_FILE}" | cut -f "${INDEX}" -d " ")

# Sign TF-A artifact
if [ "${ARTIFACT_TFA}" = "y" ]; then
	if [ "${PLATFORM}" = "ccmp15" ]; then
		SOC_OPTIONS="-hv 1"
	elif [ "${PLATFORM}" = "ccmp13" ]; then
		SOC_OPTIONS="-hv 2 -of 0x00000001"
	fi
	STM32MP_SigningTool_CLI -bin ${INPUT_FILE} \
				--public-key ${PUBLIC_KEY} \
				--private-key ${PRIVATE_KEY} \
				-t fsbl \
				-s \
				${SOC_OPTIONS} \
				--password ${PASS} \
				-o ${OUTPUT_FILE}
fi
