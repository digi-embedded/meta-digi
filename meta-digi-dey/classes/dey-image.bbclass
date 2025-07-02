# DEY image features.
#
# Copyright (C) 2012-2024, Digi International Inc.
#

#
# Add build info to rootfs images (/etc/buildinfo)
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
# Generate ZIP installer unless disabled in the project's local.conf
#
DEY_IMAGE_INSTALLER ?= "1"
inherit ${@oe.utils.conditional("DEY_IMAGE_INSTALLER", "1", "dey-image-installer", "", d)}

#
# Inherit common DEY SDK traits
#
inherit dey-image-sdk

create_sw_versions_file() {
	local swversionsfile="${IMAGE_ROOTFS}${sysconfdir}/sw-versions"

	rm -f $swversionsfile
	touch $swversionsfile
	echo 'firmware ${DEY_FIRMWARE_VERSION}' >> $swversionsfile
}
ROOTFS_POSTPROCESS_COMMAND:append = " create_sw_versions_file"

#
# Add dependency for read-only signed rootfs and SWU public key copying
#
DEPENDS += "${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'trustfence-sign-tools-native', '', d)}"

# Do not include kernel in rootfs images
PACKAGE_EXCLUDE = "kernel-image-*"

# Add required methods to generate the correct SWU update package.
inherit dey-swupdate
