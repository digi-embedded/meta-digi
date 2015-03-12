#
# Helper class for automated testing in SARES
#
# Copyright (C) 2015 Digi International.
#
# To enable, use INHERIT in local.conf:
#
#     INHERIT += "sares-image"
#

IMAGE_FEATURES += "dey-debug dey-examples"

sares() {
	##################################################################
	## WARNING: enable passwordless 'root' autologin in serial console
	## and telnetd for testing purposes
	##################################################################
	if [ -f "${IMAGE_ROOTFS}/etc/inittab" ]; then
		cat >${IMAGE_ROOTFS}/sbin/rootlogin <<-_EOF_
			#!/bin/sh
			exec /bin/login -f root
		_EOF_
		chmod u+x ${IMAGE_ROOTFS}/sbin/rootlogin

		# The 'echo' trick is needed because the SERIAL_CONSOLES variable
		# is expanded by bitbake and contains a semicolon ';'
		for i in $(echo "${SERIAL_CONSOLES}"); do
			label="$(echo $i | sed -e 's,.*tty\(.*\),\1,g')"
			sed -i -e "
				/^$label:.*getty/{
					i\## WARNING: passwordless 'root' autologin enabled
					s,getty,getty -n -l /sbin/rootlogin,g
				}" ${IMAGE_ROOTFS}/etc/inittab
		done

		# Install a telnetd daemon if there isn't one
		if ! grep -qs telnetd ${IMAGE_ROOTFS}/etc/inittab; then
			cat >>${IMAGE_ROOTFS}/etc/inittab <<-_EOF_
				## WARNING: passwordless 'root' telnet daemon
				~~::sysinit:/usr/sbin/telnetd -l /sbin/rootlogin
			_EOF_
		fi
	fi
}

IMAGE_PREPROCESS_COMMAND += "sares;"
