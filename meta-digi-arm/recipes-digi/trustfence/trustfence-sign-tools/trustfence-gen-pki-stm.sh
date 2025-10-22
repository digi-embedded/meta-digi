#!/bin/sh
#===============================================================================
#
#  trustfence-gen-pki-stm.sh
#
#  Copyright (C) 2023,2025 by Digi International Inc.
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
#    The following environment variables define the script behaviour:
#      CONFIG_SIGN_KEYS_PATH: (mandatory) Path to the folder to hold the generated PKI tree keys.
#      CONFIG_FIP_ENCRYPT_KEYNAME: (optional) Encryption key filename for FIP
#      CONFIG_FSBL_ENCRYPT_KEYNAME: (optional) Encryption key filename for FSBL
#      CONFIG_RPROC_ENCRYPT_KEYNAME: (optional) Encryption key filename for RPROC
#
#===============================================================================

# Avoid parallel execution of this script
SINGLE_PROCESS_LOCK="/tmp/gen_pki_script.lock.d"
trap 'rm -rf "${SINGLE_PROCESS_LOCK}"' INT TERM EXIT
while ! mkdir "${SINGLE_PROCESS_LOCK}" > /dev/null 2>&1; do
	sleep 1
done

SCRIPT_NAME="$(basename "${0}")"

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
    -p <platform>    platform (such as ccmp15, ccmp13, ccmp25...)


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
else
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
fi

# Default values
RPROC_KEY_PASS_FILE="${CONFIG_SIGN_KEYS_PATH}/rproc-keys/key_pass.txt"

# Generate random keys for Cortex-M coprocessor if they don't exist
if [ "${PLATFORM}" = "ccmp25" ]; then
	N_PUBK="$(ls -l ${CONFIG_SIGN_KEYS_PATH}/rproc-keys/publicKey*.pem 2>/dev/null | wc -l)"
	N_PRVK="$(ls -l ${CONFIG_SIGN_KEYS_PATH}/rproc-keys/privateKey*.pem 2>/dev/null | wc -l)"
	N_DERK="$(ls -l ${CONFIG_SIGN_KEYS_PATH}/rproc-keys/publicKey*.der 2>/dev/null | wc -l)"
	install -d "${CONFIG_SIGN_KEYS_PATH}/rproc-keys/"
	if [ "${N_PUBK}" = "1" ] && [ "${N_PRVK}" = "1" ] && [ "${N_DERK}" = "1" ] && [ -f "${RPROC_KEY_PASS_FILE}" ]; then
		# PKI tree already exists.
		echo "Using existing PKI tree for Cortex-M coprocessor"
	elif [ "${N_PUBK}" != "1" ] && [ "${N_PRVK}" != 1 ] && [ "${N_DERK}" != "1" ] && [ ! -f "${RPROC_KEY_PASS_FILE}" ]; then
		# Random password
		password="$(openssl rand -base64 32)"
		echo "Generating random key"
		if ! STM32MP_KeyGen_CLI -abs "${CONFIG_SIGN_KEYS_PATH}/rproc-keys/" -pwd ${password}; then
			echo "[ERROR] Could not generate PKI tree for Cortex-M coprocessor"
			exit 1
		fi
		echo "${password}" > "${RPROC_KEY_PASS_FILE}"
		chmod 400 "${RPROC_KEY_PASS_FILE}"
		# Generate DER version of public key
		if ! openssl ec -pubin -in ${CONFIG_SIGN_KEYS_PATH}/rproc-keys/publicKey.pem \
		           -outform DER -pubout \
		           -out ${CONFIG_SIGN_KEYS_PATH}/rproc-keys/publicKey.der; then
			echo "[ERROR] Could not generate DER public key for Cortex-M coprocessor"
			exit 1
		fi
	else
		echo "[ERROR] Could not generate PKI tree for Cortex-M coprocessor. An incomplete PKI tree may already exist."
		exit 1
	fi
fi

if [ -n "${CONFIG_FSBL_ENCRYPT_KEYNAME}" ] && [ -n "${CONFIG_FIP_ENCRYPT_KEYNAME}" ] && [ -n "${CONFIG_RPROC_ENCRYPT_KEYNAME}" ]; then

	# Generate random keys if they don't exist
	if [ "${PLATFORM}" = "ccmp25" ]; then
		if [ ! -f "${CONFIG_SIGN_KEYS_PATH}/${CONFIG_FSBL_ENCRYPT_KEYNAME}" ]; then
			echo "Generating random encryption key for FSBL"
			if ! STM32MP_KeyGen_CLI -rand 16 "${CONFIG_SIGN_KEYS_PATH}/${CONFIG_FSBL_ENCRYPT_KEYNAME}"; then
				echo "[ERROR] Failed to generate 16-byte FSBL encryption key"
				exit 1
			fi
			chmod 444 "${CONFIG_SIGN_KEYS_PATH}/${CONFIG_FSBL_ENCRYPT_KEYNAME}"
		fi
		if [ ! -f "${CONFIG_SIGN_KEYS_PATH}/${CONFIG_FIP_ENCRYPT_KEYNAME}" ]; then
			echo "Generating random encryption key for FIP"
			if ! STM32MP_KeyGen_CLI -rand 32 "${CONFIG_SIGN_KEYS_PATH}/${CONFIG_FIP_ENCRYPT_KEYNAME}"; then
				echo "[ERROR] Failed to generate 32-byte FIP encryption key"
				exit 1
			fi
			chmod 444 "${CONFIG_SIGN_KEYS_PATH}/${CONFIG_FIP_ENCRYPT_KEYNAME}"
		fi
		if [ ! -f "${CONFIG_SIGN_KEYS_PATH}/${CONFIG_RPROC_ENCRYPT_KEYNAME}" ]; then
			echo "Generating random encryption keys for Cortex-M coprocessor"
			if ! STM32MP_KeyGen_CLI -rand 32 "${CONFIG_SIGN_KEYS_PATH}/${CONFIG_RPROC_ENCRYPT_KEYNAME}"; then
				echo "[ERROR] Failed to generate 32-byte Cortex-M encryption key"
				exit 1
			fi
			chmod 444 "${CONFIG_SIGN_KEYS_PATH}/${CONFIG_RPROC_ENCRYPT_KEYNAME}"
		fi
	else
		echo "[ERROR] Could not generate encryption keys. Platform not supported."
		exit 1
	fi
fi
