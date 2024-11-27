# Copyright (C) 2024, Digi International Inc.

#
# Create a dey-version file when populating the toolchain/SDK
#
# 'SDK_POSTPROCESS_COMMAND' variable is originally defined in populate_sdk_base
# class: poky/meta/classes/populate_sdk_base.bbclass
# It is redefined here to be able to tweak the resulting SDK before packaging,
# using the proper 'IMAGE_BASENAME' value.
#
SDK_PREPACKAGING_COMMAND ?= "toolchain_create_sdk_dey_version"
SDK_POSTPROCESS_COMMAND = " create_sdk_files check_sdk_sysroots ${SDK_PREPACKAGING_COMMAND} archive_sdk ${SDK_PACKAGING_COMMAND} "

# This function creates a DEY version information file
fakeroot toolchain_create_sdk_dey_version() {
	local deyversionfile="${SDK_OUTPUT}/${SDKPATH}/dey-version-${REAL_MULTIMACH_TARGET_SYS}"

	rm -f $deyversionfile
	touch $deyversionfile
	echo 'Machine: ${MACHINE}' >> $deyversionfile
	echo 'Version: ${DISTRO_VERSION}-${DATETIME}' >> $deyversionfile
	echo 'Image: ${IMAGE_BASENAME}' >> $deyversionfile
}
toolchain_create_sdk_dey_version[vardepsexclude] = "DATETIME"

# Add staticdev packages to SDK
SDKIMAGE_FEATURES:append = " staticdev-pkgs"
