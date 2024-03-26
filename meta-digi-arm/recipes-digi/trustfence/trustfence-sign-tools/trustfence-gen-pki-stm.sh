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
KEY_PASS_BASEFILE="${CONFIG_SIGN_KEYS_PATH}/keys/key_pass"
KEY_PASS_FILE="${KEY_PASS_BASEFILE}.txt"

# Generate random keys if they don't exist
N_PUBK="$(ls -l ${CONFIG_SIGN_KEYS_PATH}/keys/publicKey*.pem 2>/dev/null | wc -l)"
N_PRVK="$(ls -l ${CONFIG_SIGN_KEYS_PATH}/keys/privateKey*.pem 2>/dev/null | wc -l)"
N_PASS="$(ls -l ${KEY_PASS_BASEFILE}*.txt 2>/dev/null | wc -l)"
install -d "${CONFIG_SIGN_KEYS_PATH}/keys/"
if [ "${PLATFORM}" = "ccmp15" ]; then
	if [ "${N_PUBK}" != "1" ] && [ "${N_PRVK}" != 1 ] && [ ! -f "${KEY_PASS_FILE}" ]; then
		# Random password
		password="$(openssl rand -base64 32)"
		echo "Generating random key"
		if ! STM32MP_KeyGen_CLI -abs "${CONFIG_SIGN_KEYS_PATH}/keys/" -pwd ${password}; then
			echo "[ERROR] Could not generate PKI tree"
			exit 1
		fi
		echo "${password}" > "${KEY_PASS_FILE}"
		chmod 400 "${KEY_PASS_FILE}"
	fi
elif [ "${PLATFORM}" = "ccmp13" ]; then
	if [ "${N_PUBK}" = "8" ] && [ "${N_PRVK}" = "8" ] && [ "${N_PASS}" = "8" ]; then
		# PKI tree already exists.
		echo "Using existing PKI tree"
	elif [ "${N_PUBK}" = "8" ] && [ "${N_PRVK}" = "8" ] && [ "${N_PASS}" != "8" ] && [ -f "${KEY_PASS_FILE}" ]; then
		# Backwards compatibility: if a single key_pass.txt file exists,
		# split into 8 files with one password each
		for i in $(seq 0 7); do
			cat "${KEY_PASS_FILE}" | cut -f $((i+1)) -d " " > "${KEY_PASS_BASEFILE}0${i}.txt"
			chmod 400 "${KEY_PASS_BASEFILE}0${i}.txt"
		done
	elif [ "${N_PUBK}" != "8" ] && [ "${N_PRVK}" != "8" ] && [ "${N_PASS}" != "8" ]; then
		# Generate 8 random passwords
		for i in $(seq 0 7); do
			pass="$(openssl rand -base64 32)"
			echo "${pass}" > "${KEY_PASS_BASEFILE}0${i}.txt"
			chmod 400 "${KEY_PASS_BASEFILE}0${i}.txt"
			# Combined string with 8 passwords separated by a white space
			passwords="${passwords} ${pass}"
		done
		echo "Generating random keys"
		if ! STM32MP_KeyGen_CLI -abs "${CONFIG_SIGN_KEYS_PATH}/keys/" -pwd ${passwords} -n 8; then
			echo "[ERROR] Could not generate PKI tree"
			exit 1
		fi
	else
		echo "[ERROR] Could not generate PKI tree. An incomplete PKI tree may already exist."
		exit 1
	fi
else
	echo "Undefined platform"
	exit 1
fi
