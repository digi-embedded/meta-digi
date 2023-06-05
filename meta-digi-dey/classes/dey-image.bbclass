# DEY image features.
#
# Copyright (C) 2012 Digi International.

#
# Add build info to rootfs images (/etc/build)
#
inherit image-buildinfo

#
# Set root password using 'extrausers' class if 'debug-tweaks' is NOT enabled
#
# To get the hash of the password (with escaped '$' char: '\$') run
# the following command in your development computer:
#
#   echo -n 'root' | mkpasswd -5 -s | sed -e 's,\$,\\$,g'
#
inherit ${@bb.utils.contains("IMAGE_FEATURES", "debug-tweaks", "", "extrausers",d)}

MD5_ROOT_PASSWD ?= "\$1\$SML0de4S\$lOWs3t82QAH0oEf8NyNKA0"
EXTRA_USERS_PARAMS += "\
    usermod -p '${MD5_ROOT_PASSWD}' root; \
"

#
# Create QT5 capable toolchain/SDK if 'dey-qt' image feature is enabled
#
inherit qt-version
inherit ${@bb.utils.contains("IMAGE_FEATURES", "dey-qt", "${QT_POPULATE_SDK}", "",d)}

#
# Generate ZIP installer if configured in the project's local.conf
#
DEY_IMAGE_INSTALLER ?= "0"
inherit ${@oe.utils.conditional("DEY_IMAGE_INSTALLER", "1", "dey-image-installer", "", d)}

#
# Create a dey-version file when populating the toolchain/SDK
#
# 'SDK_POSTPROCESS_COMMAND' variable is originally defined in populate_sdk_base
# class: poky/meta/classes/populate_sdk_base.bbclass
# It is redefined here to be able to tweak the resulting SDK before packaging,
# using the proper 'IMAGE_BASENAME' value.
#
SDK_PREPACKAGING_COMMAND ?= "toolchain_create_sdk_dey_version"
SDK_POSTPROCESS_COMMAND = " create_sdk_files; check_sdk_sysroots; ${SDK_PREPACKAGING_COMMAND}; archive_sdk; ${SDK_PACKAGING_COMMAND} "

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

create_sw_versions_file() {
	local swversionsfile="${IMAGE_ROOTFS}${sysconfdir}/sw-versions"

	rm -f $swversionsfile
	touch $swversionsfile
	echo 'firmware ${DEY_FIRMWARE_VERSION}' >> $swversionsfile
}
ROOTFS_POSTPROCESS_COMMAND:append = " create_sw_versions_file;"

#
# Add dependency for read-only signed rootfs
#
DEPENDS += "${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'trustfence-sign-tools-native', '', d)}"

# Do not include kernel in rootfs images
PACKAGE_EXCLUDE = "kernel-image-*"
