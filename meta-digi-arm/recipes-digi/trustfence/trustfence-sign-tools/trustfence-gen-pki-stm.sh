#!/bin/sh
#===============================================================================
#
#  trustfence-gen-pki-stm.sh
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
#    Script for generating PKI tree using STM tools
#
#===============================================================================

# Avoid parallel execution of this script
SINGLE_PROCESS_LOCK="/tmp/gen_pki_script.lock.d"
trap 'rm -rf "${SINGLE_PROCESS_LOCK}"' INT TERM EXIT
while ! mkdir "${SINGLE_PROCESS_LOCK}" > /dev/null 2>&1; do
	sleep 1
done

SCRIPT_NAME="$(basename "${0}")"
SUPPORTED_PLATFORMS="ccmp15, ccmp13"

while getopts "p:" c; do
	case "${c}" in
		p) PLATFORM="${OPTARG}";;
	esac
done
shift "$((OPTIND - 1))"

usage() {
	cat <<EOF

Usage: ${SCRIPT_NAME} <OPTIONS>

 Options:
    -p <platform>    platform

Supported platforms: ${SUPPORTED_PLATFORMS}

EOF
}

if [ -z "${CONFIG_SIGN_KEYS_PATH}" ]; then
	echo "Undefined CONFIG_SIGN_KEYS_PATH";
	exit 1
fi
[ -d "${CONFIG_SIGN_KEYS_PATH}" ] || mkdir "${CONFIG_SIGN_KEYS_PATH}"

# Default values
KEY_PASS_FILE="${CONFIG_SIGN_KEYS_PATH}/keys/key_pass.txt"

# Generate random keys if they don't exist
N_PUBK="$(ls -l "${CONFIG_SIGN_KEYS_PATH}"/keys/publicKey0* 2>/dev/null | wc -l)"
N_PRVK="$(ls -l "${CONFIG_SIGN_KEYS_PATH}"/keys/privateKey0* 2>/dev/null | wc -l)"
if [ "${PLATFORM}" = "ccmp15" ]; then
	if [ "${N_PUBK}" != "1" ] && [ "${N_PRVK}" != 1 ] && [ ! -f "${KEY_PASS_FILE}" ]; then
		install -d "${CONFIG_SIGN_KEYS_PATH}/keys/"
		# Random password
		password="$(openssl rand -base64 32)"
		echo "Generating random key"
		if ! STM32MP_KeyGen_CLI -abs "${CONFIG_SIGN_KEYS_PATH}/keys/" -pwd ${password} -n 1; then
			echo "[ERROR] Could not generate PKI tree"
			exit 1
		fi
		echo "${password}" > "${KEY_PASS_FILE}"
	fi
elif [ "${PLATFORM}" = "ccmp13" ]; then
	if [ "${N_PUBK}" != "8" ] && [ "${N_PRVK}" != 8 ] && [ ! -f "${KEY_PASS_FILE}" ]; then
		install -d "${CONFIG_SIGN_KEYS_PATH}/keys/"
		# 8 random passwords (separated by whitespaces)
		passwords="$(openssl rand -base64 32)"
		for i in $(seq 1 7); do
			passwords="${passwords} $(openssl rand -base64 32)"
		done
		echo "Generating random keys"
		if ! STM32MP_KeyGen_CLI -abs "${CONFIG_SIGN_KEYS_PATH}/keys/" -pwd ${passwords} -n 8; then
			echo "[ERROR] Could not generate PKI tree"
			exit 1
		fi
		echo "${passwords}" > "${KEY_PASS_FILE}"
	fi
else
	echo "Undefined platform"
	exit 1
fi
