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

MKP_SCRIPTNAME="$(basename ${BASH_SOURCE})"
MKP_SCRIPTPATH="$(cd $(dirname ${BASH_SOURCE}) && pwd)"
MKP_PROJECTPATH="$(pwd)"

## Color codes
MKP_RED="\033[1;31m"
MKP_GREEN="\033[1;32m"
MKP_NONE="\033[0m"

# Path to platform config files
MKP_CONFIGPATH="${MKP_SCRIPTPATH}/sources/meta-digi/sdk/config"

# Blacklist platforms (not officially supported in a DEY release)
MKP_BLACKLIST_PLATFORMS="ccardimx28js"

MKP_SETUP_ENVIRONMENT='#!/bin/bash

if [ "${BASH_SOURCE}" = "${0}" ]; then
	printf "\\n[ERROR]: This script needs to be sourced\\n\\n"
else
	cd $(dirname ${BASH_SOURCE})
	. %s/sources/poky/oe-init-build-env .
fi
'

## Local functions
usage() {
	cat <<EOF

Usage: source ${MKP_SCRIPTNAME} [OPTIONS]

    -l               list supported platforms
    -p <platform>    select platform for the project
    -v <variant>     select platform variant

Supported platforms: $(display_supported_platforms)

See platform include files for supported variant names:

${MKP_SCRIPTPATH}/sources/meta-digi/meta-digi-arm/conf/machine/include/<platform>.inc

EOF
}

error() {
	if [ ${#} -ne 0 ] ; then
		printf "\n${MKP_RED}[ERROR]:${MKP_NONE} %s\n" "${1}"
	fi
	usage
}

check_selected_platform() {
	for i in : ${MKP_AVAILABLE_PLATFORMS}; do
		[ "${i}" = ":" ] && continue
		[ "${i}" = "${MKP_PLATFORM}" ] && return 0
	done
	return 1
}

# Filter available platforms through the blacklist
display_supported_platforms() {
	local MKP_SUPPORTED_PLATFORMS=""
	for i in ${MKP_AVAILABLE_PLATFORMS}; do
		if echo "${MKP_BLACKLIST_PLATFORMS}" | grep -qsv "${i}"; then
			MKP_SUPPORTED_PLATFORMS="${MKP_SUPPORTED_PLATFORMS:+${MKP_SUPPORTED_PLATFORMS} }${i}"
		fi
	done
	echo "${MKP_SUPPORTED_PLATFORMS}"
	unset MKP_SUPPORTED_PLATFORMS
}

do_license() {
	local MKP_LICENSE_FILES=" \
		${MKP_SCRIPTPATH}/sources/meta-digi/meta-digi-arm/DIGI_EULA \
		${MKP_SCRIPTPATH}/sources/meta-digi/meta-digi-arm/DIGI_OPEN_EULA \
		${MKP_SCRIPTPATH}/sources/meta-fsl-arm/EULA \
	"
	[ -z "${MKP_PAGER+x}" ] && MKP_PAGER="| more"
	eval cat - "${MKP_LICENSE_FILES}" <<-_EOF_ ${MKP_PAGER}; printf "\n"
		+-------------------------------------------------------------------------------+
		|                                                                               |
		|                                                                               |
		|  This software depends on libraries and packages that are covered by the      |
		|  following licenses:                                                          |
		|                                                                               |
		|      * Digi's end user license agreement                                      |
		|      * Digi's third party and open source license notice                      |
		|      * Freescale semiconductor software license agreement                     |
		|                                                                               |
		|  To have the right to use those binaries in your images you need to read and  |
		|  accept the licenses.                                                         |
		|                                                                               |
		|                                                                               |
		+-------------------------------------------------------------------------------+

	_EOF_
	unset MKP_LICENSE_FILES MKP_PAGER

	ans=""
	while [ -z "${ans}" ]; do
		read -p "Do you accept all three license agreements? [y/Y to accept]: " ans
	done
	printf "%80s\n\n" | tr ' ' '-'

	[ "${ans,,}" = "y" ] || return 1
}

do_mkproject() {
	export TEMPLATECONF="${TEMPLATECONF:-${MKP_CONFIGPATH}/${MKP_PLATFORM}}"
	source ${MKP_SCRIPTPATH}/sources/poky/oe-init-build-env .
	unset TEMPLATECONF

	# New project
	if [ -z "${MKP_OLD_PROJECT}" ]; then
		# Customize project
		chmod 644 ${MKP_PROJECTPATH}/conf/bblayers.conf ${MKP_PROJECTPATH}/conf/local.conf
		sed -i -e "s,##DIGIBASE##,${MKP_SCRIPTPATH}/sources,g" ${MKP_PROJECTPATH}/conf/bblayers.conf
		if [ -n "${MKP_VARIANT+x}" ]; then
			sed -i -e "/^MACHINE_VARIANT =/cMACHINE_VARIANT = \"${MKP_VARIANT}\"" \
				${MKP_PROJECTPATH}/conf/local.conf
		fi
		# At this point the user has accepted all the licenses, so enable the FSL EULA
		sed -i -e "s,^#ACCEPT_FSL_EULA,ACCEPT_FSL_EULA,g" ${MKP_PROJECTPATH}/conf/local.conf
		# Create dey-setup-environment script
		printf "${MKP_SETUP_ENVIRONMENT}" "${MKP_SCRIPTPATH}" > ${MKP_PROJECTPATH}/dey-setup-environment
		chmod +x ${MKP_PROJECTPATH}/dey-setup-environment
	fi
}

# Keep the running script in sync with the one in the layer. If it differs,
# update it (copy/overwrite) and warn the user.
if ! cmp -s ${MKP_SCRIPTPATH}/${MKP_SCRIPTNAME} ${MKP_SCRIPTPATH}/sources/meta-digi/sdk/${MKP_SCRIPTNAME}; then
	install -m 0555 ${MKP_SCRIPTPATH}/sources/meta-digi/sdk/${MKP_SCRIPTNAME} ${MKP_SCRIPTPATH}/${MKP_SCRIPTNAME}
	printf "\n${MKP_GREEN}[INFO]:${MKP_NONE} %s\n" "the '${MKP_SCRIPTNAME}' script has been updated."
	printf "\nPlease run '. ${BASH_SOURCE}' again.\n\n"
	return
fi

## Get available platforms
MKP_AVAILABLE_PLATFORMS="$(echo $(ls -1 ${MKP_CONFIGPATH}/*/local.conf.sample | sed -e 's,^.*config/\([^/]\+\)/local\.conf\.sample,\1,g'))"

# Verify if this is a new project (so we do NOT customize it)
[ -r "${MKP_PROJECTPATH}/conf/bblayers.conf" -a -r "${MKP_PROJECTPATH}/conf/local.conf" ] && MKP_OLD_PROJECT="1"

# The script needs to be sourced (not executed) so make sure to
# initialize OPTIND variable for getopts.
OPTIND=1
while getopts "lp:v:" c; do
	case "${c}" in
		l) MKP_LIST_PLATFORMS="y";;
		p) MKP_PLATFORM="${OPTARG}";;
		v) MKP_VARIANT="${OPTARG}";;
	esac
done

## Sanity checks
if [ "${BASH_SOURCE}" = "${0}" ]; then
	error "This script needs to be sourced"
elif [ ${#} -eq 0 ] ; then
	usage
elif [ -n "${MKP_LIST_PLATFORMS}" ]; then
	display_supported_platforms
elif [ -z "${MKP_PLATFORM}" ]; then
	error "-p option is required"
elif ! check_selected_platform; then
	error "the selected platform \"${MKP_PLATFORM}\" is not available"
else
	do_license && do_mkproject || printf "License NOT accepted. Make project cancelled.\n\n"
fi

# clean-up all variables (so the script can be re-sourced)
unset MKP_AVAILABLE_PLATFORMS \
      MKP_BLACKLIST_PLATFORMS \
      MKP_GREEN \
      MKP_LIST_PLATFORMS \
      MKP_NONE \
      MKP_OLD_PROJECT \
      MKP_PLATFORM \
      MKP_PROJECTPATH \
      MKP_RED \
      MKP_SCRIPTNAME \
      MKP_SCRIPTPATH \
      MKP_SETUP_ENVIRONMENT \
      MKP_VARIANT
