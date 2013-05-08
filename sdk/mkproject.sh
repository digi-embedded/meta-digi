#!/bin/bash
#===============================================================================
#
#  mkproject.sh
#
#  Copyright (C) 2013 by Digi International Inc.
#  All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License version 2 as published by
#  the Free Software Foundation.
#
#
#  !Description: Yocto project maker (for Digi's SDK)
#
#===============================================================================

SCRIPTNAME="$(basename ${BASH_SOURCE})"
SCRIPTPATH="$(cd $(dirname ${BASH_SOURCE}) && pwd)"
PROJECTPATH="$(pwd)"

## Color codes
RED="\033[1;31m"
GREEN="\033[1;32m"
NONE="\033[0m"

# Path to platform config files
CONFIGPATH="${SCRIPTPATH}/sources/meta-digi/sdk/config"

## Local functions
usage() {
	printf "\nUsage: source ${SCRIPTNAME} [OPTIONS]\n
	-l               list available platforms
	-p <platform>    select platform for the project
	\n"

	printf "Available platforms: ${AVAILABLE_PLATFORMS}\n\n"
}

error() {
	if [ ${#} -ne 0 ] ; then
		printf "\n${RED}[ERROR]:${NONE} %s\n" "${1}"
	fi
	usage
}

check_selected_platform() {
	for i in : ${AVAILABLE_PLATFORMS}; do
		[ "${i}" = ":" ] && continue
		[ "${i}" = "${platform}" ] && return 0
	done
	return 1
}

do_mkproject() {
	export TEMPLATECONF="${CONFIGPATH}/${platform}"
	source ${SCRIPTPATH}/sources/poky/oe-init-build-env .
	unset TEMPLATECONF

	# Remove possible duplicated entries in PATH (due to re-sourcing the script)
	export PATH=$(printf ${PATH} | awk -v RS=: '{if (!arr[$0]++) {printf("%s%s", !ln++ ? "" : ":", $0) }}')

	# Customize project if just created
	if [ -z "${OLD_PROJECT}" ]; then
		NCPU="$(grep -c processor /proc/cpuinfo)"
		chmod 644 ${PROJECTPATH}/conf/bblayers.conf ${PROJECTPATH}/conf/local.conf
		sed -i  -e"s,##DIGIBASE##,${SCRIPTPATH}/sources,g" ${PROJECTPATH}/conf/bblayers.conf
		sed -i  -e "/^#BB_NUMBER_THREADS =/cBB_NUMBER_THREADS = \"${NCPU}\"" \
			-e "/^#PARALLEL_MAKE =/cPARALLEL_MAKE = \"-j ${NCPU}\"" \
			${PROJECTPATH}/conf/local.conf
		unset NCPU
	fi
}

# Keep the running script in sync with the one in the layer. If it differs,
# update it (copy/overwrite) and warn the user.
if ! cmp -s ${SCRIPTPATH}/${SCRIPTNAME} ${SCRIPTPATH}/sources/meta-digi/sdk/${SCRIPTNAME}; then
	install -m 0555 ${SCRIPTPATH}/sources/meta-digi/sdk/${SCRIPTNAME} ${SCRIPTPATH}/${SCRIPTNAME}
	printf "\n${GREEN}[INFO]:${NONE} %s\n" "the '${SCRIPTNAME}' script has been updated."
	printf "\nPlease run '. ${BASH_SOURCE}' again.\n\n"
	return
fi

## Get available platforms
AVAILABLE_PLATFORMS="$(echo $(ls -1 ${CONFIGPATH}/*/local.conf.sample | sed -e 's,^.*config/\([^/]\+\)/local\.conf\.sample,\1,g'))"

# Verify if this is a new project (so we do NOT customize it)
[ -r "${PROJECTPATH}/conf/bblayers.conf" -a -r "${PROJECTPATH}/conf/local.conf" ] && OLD_PROJECT="1"

# The script needs to be sourced (not executed) so make sure to
# initialize OPTIND variable for getopts.
OPTIND=1
while getopts "lp:" c; do
	case "${c}" in
		l) list_platforms="y";;
		p) platform="${OPTARG}";;
	esac
done

## Sanity checks
if [ "${BASH_SOURCE}" = "${0}" ]; then
	error "This script needs to be sourced"
elif [ ${#} -eq 0 ] ; then
	usage
elif [ -n "${list_platforms}" ]; then
        echo ${AVAILABLE_PLATFORMS}
elif [ -z "${platform}" ]; then
        error "-p option is required"
elif ! check_selected_platform; then
        error "the selected platform \"${platform}\" is not available"
else
	do_mkproject
fi

# clean-up all variables (so the script can be re-sourced)
unset AVAILABLE_PLATFORMS GREEN NONE OLD_PROJECT PROJECTPATH RED SCRIPTNAME SCRIPTPATH
unset list_platforms platform
