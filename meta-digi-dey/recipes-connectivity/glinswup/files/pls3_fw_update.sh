#!/bin/sh
#===============================================================================
#
#  Copyright (C) 2026 by Digi International Inc.
#  All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License version 2 as published by
#  the Free Software Foundation.
#
#
#  Description:
#    Update firmware on a Cinterion PLSx3 modem using glinswup_PLSx3.
#
#
#  Usage:
#    pls3_fw_update.sh -f <firmware.usf> -p <port> [-n] [-v]
#
#===============================================================================

set -eu

UPDATE_BIN="glinswup_PLSx3"
MM_SERVICE_SYSTEMD=${MM_SERVICE_SYSTEMD:-ModemManager}

# On sysvinit-based platforms, NetworkManager respawns ModemManager automatically.
# Therefore we must stop NetworkManager to ensure ModemManager remains stopped
# during the firmware update process.
NM_SERVICE_VINIT=${NM_SERVICE_VINIT:-networkmanager}

FW_FILE=""
PORT=""
VERBOSE=0
RECOVERY=0

exit_error()
{
	ERROR_MESSAGE="$1"
	ERROR_CODE="${2:-1}"

	echo "## ERROR: ${ERROR_MESSAGE}" >&2
	exit "${ERROR_CODE}"
}

log_info()
{
	echo "## INFO: $*"
}

stop_services()
{
	if command -v systemctl >/dev/null 2>&1; then
		log_info "Stopping ${MM_SERVICE_SYSTEMD}..."
		systemctl stop "${MM_SERVICE_SYSTEMD}" >/dev/null 2>&1 || true
		return 0
	fi

	if [ -x "/etc/init.d/${NM_SERVICE_VINIT}" ]; then
		log_info "Stopping ${NM_SERVICE_VINIT}..."
		"/etc/init.d/${NM_SERVICE_VINIT}" stop >/dev/null 2>&1 || true
		return 0
	fi

	log_info "No service manager found; skipping stop/start"
	return 0
}

start_services()
{
	if command -v systemctl >/dev/null 2>&1; then
		systemctl start "${MM_SERVICE_SYSTEMD}" >/dev/null 2>&1 || true
		return 0
	fi

	if [ -x "/etc/init.d/${NM_SERVICE_VINIT}" ]; then
		"/etc/init.d/${NM_SERVICE_VINIT}" start >/dev/null 2>&1 || true
		return 0
	fi

	return 0
}

usage()
{
	cat <<'EOF'
Usage:
  pls3_fw_update.sh -f <firmware.usf> -p <port> [-n] [-v]

Options:
  -f, --firmware   Path to firmware file (.usf)
  -p, --port       Serial port used by the updater tool (e.g. /dev/ttyACM0)
  -n               Recovery mode (pass -n to updater)
  -v               Verbose (pass -v to updater and show output)

Environment:
  MM_SERVICE_SYSTEMD=ModemManager
  NM_SERVICE_VINIT=networkmanager
EOF
}

parse_args()
{
	while [ $# -gt 0 ]; do
		case "$1" in
			-f|--firmware)
				[ $# -ge 2 ] || exit_error "Missing value for $1" 2
				FW_FILE="${2:-}"; shift 2;;
			-p|--port)
				[ $# -ge 2 ] || exit_error "Missing value for $1" 2
				PORT="${2:-}"; shift 2;;
			-n)
				RECOVERY=1; shift;;
			-v)
				VERBOSE=1; shift;;
			-h|--help)
				usage; exit 0;;
			*)
				exit_error "Unknown argument: $1" 2;;
		esac
	done
}

require_prereqs()
{
	[ -n "${FW_FILE}" ] || exit_error "Missing -f/--firmware" 2
	[ -n "${PORT}" ] || exit_error "Missing -p/--port" 2

	[ -r "${FW_FILE}" ] || exit_error "Firmware file not readable: ${FW_FILE}" 2
	command -v "${UPDATE_BIN}" >/dev/null 2>&1 || exit_error "Update tool not found in PATH: ${UPDATE_BIN}" 2
	[ -e "${PORT}" ] || exit_error "Port does not exist: ${PORT}" 2
}

run_update()
{
	RC=0
	VERBOSE_ARG=""
	RECOVERY_ARG=""

	[ "${VERBOSE}" -eq 1 ] && VERBOSE_ARG="-v"
	[ "${RECOVERY}" -eq 1 ] && RECOVERY_ARG="-n"

	log_info "Running: ${UPDATE_BIN} -f '${FW_FILE}' -p '${PORT}' -u 1 ${RECOVERY_ARG} ${VERBOSE_ARG}"

	if [ "${VERBOSE}" -eq 1 ]; then
		"${UPDATE_BIN}" -f "${FW_FILE}" -p "${PORT}" -u 1 ${RECOVERY_ARG} ${VERBOSE_ARG} || RC=$?
	else
		"${UPDATE_BIN}" -f "${FW_FILE}" -p "${PORT}" -u 1 ${RECOVERY_ARG} >/dev/null 2>&1 || RC=$?
	fi

	if [ "${RC}" -ne 0 ]; then
		exit_error "Firmware update FAILED (exit code: ${RC})" "${RC}"
	fi

	log_info "Firmware update SUCCESS"
}

parse_args "$@"
require_prereqs

stop_services

# Always try to restore services even if the update fails.
trap 'start_services' EXIT
run_update
