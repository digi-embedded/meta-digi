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
