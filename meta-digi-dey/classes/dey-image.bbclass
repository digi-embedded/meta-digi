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
# To get the encrypted password (with escaped '$' char: '\$') run following
# command in your development computer:
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
inherit ${@bb.utils.contains("IMAGE_FEATURES", "dey-qt", "populate_sdk_qt5", "",d)}

#
# Generate ZIP installer if configured in the project's local.conf
#
DEY_IMAGE_INSTALLER ?= "0"
inherit ${@base_conditional("DEY_IMAGE_INSTALLER", "1", "dey-image-installer", "", d)}
