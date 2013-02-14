# DEL image features.
#
# Copyright (C) 2012 Digi International.

PACKAGE_GROUP_del-audio = "packagegroup-del-audio"
PACKAGE_GROUP_del-gstreamer = "packagegroup-del-gstreamer"
PACKAGE_GROUP_del-network = "packagegroup-del-network"
PACKAGE_GROUP_del-wireless = "packagegroup-del-wireless"
PACKAGE_GROUP_del-debug = "packagegroup-del-debug"
PACKAGE_GROUP_del-bluetooth = "packagegroup-del-bluetooth"
PACKAGE_GROUP_del-qt = "packagegroup-del-qt"

## DEL rootfs final tuning
del_rootfs_tuning() {
	#######################################################################
	## Set root password to 'root' if 'debug-tweaks' is NOT enabled.
	#######################################################################
	MD5_ROOT_PASSWD='$1$SML0de4S$lOWs3t82QAH0oEf8NyNKA0'
	if echo "${IMAGE_FEATURES}" | grep -qs debug-tweaks; then
		: # No-op
	else
		# Shadow passwords ENABLED
		if [ -f "${IMAGE_ROOTFS}/etc/shadow" ]; then
			sed 's%^root:[^:]*:%root:x:%' <${IMAGE_ROOTFS}/etc/passwd >${IMAGE_ROOTFS}/etc/passwd.new
			sed "s%^root:[^:]*:%root:${MD5_ROOT_PASSWD}:%" <${IMAGE_ROOTFS}/etc/shadow >${IMAGE_ROOTFS}/etc/shadow.new
			mv ${IMAGE_ROOTFS}/etc/passwd.new ${IMAGE_ROOTFS}/etc/passwd
			mv ${IMAGE_ROOTFS}/etc/shadow.new ${IMAGE_ROOTFS}/etc/shadow
		# Shadow passwords DISABLED
		else
			sed "s%^root:[^:]*:%root:${MD5_ROOT_PASSWD}:%" <${IMAGE_ROOTFS}/etc/passwd >${IMAGE_ROOTFS}/etc/passwd.new
			mv ${IMAGE_ROOTFS}/etc/passwd.new ${IMAGE_ROOTFS}/etc/passwd
		fi
	fi
	#######################################################################
	## WARNING:
	## enable passwordless 'root' autologin in serial console and telnetd
	## for testing purposes (when IMAGE_FEATURES contains 'del-test')
	#######################################################################
	if echo "${IMAGE_FEATURES}" | grep -qs del-test; then
		if [ -f "${IMAGE_ROOTFS}/etc/inittab" ]; then
			cat >${IMAGE_ROOTFS}/sbin/rootlogin <<-_EOF_
				#!/bin/sh
				exec /bin/login -f root
			_EOF_
			chmod u+x ${IMAGE_ROOTFS}/sbin/rootlogin
			sed -i -e '
				/^S.*getty/{
					i\## WARNING: passwordless 'root' autologin enabled
					a\~~::sysinit:/usr/sbin/telnetd -l /sbin/rootlogin
					s,getty,getty -n -l /sbin/rootlogin,g
				}' ${IMAGE_ROOTFS}/etc/inittab
			rm -f ${IMAGE_ROOTFS}/etc/securetty
		fi
	fi
}
