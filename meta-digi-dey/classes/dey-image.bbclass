# DEY image features.
#
# Copyright (C) 2012 Digi International.

## Auxiliar variables and functions (used in dey_rootfs_tuning)
LAYERS_REV = "${@'\n'.join(get_layers_branch_rev(d))}"
DEY_TAG    = "${@dey_tag(d).strip()}"
def dey_tag(d):
    import subprocess
    for layer in d.getVar('BBLAYERS', True).split():
        if 'meta-digi-dey' in layer:
            cmd = 'git describe --tags --exact-match 2>/dev/null || true'
            return subprocess.Popen(cmd, cwd=layer, shell=True, stdout=subprocess.PIPE).stdout.read()
    return ""

## DEY rootfs final tuning
dey_rootfs_tuning() {
	#######################################################################
	## Create '/etc/build' with build statistics
	#######################################################################
	cat >${IMAGE_ROOTFS}/etc/build <<-_EOF_
		TIMESTAMP=${DATETIME}
		DEY_TAG=${DEY_TAG}

		Layers revisions:
		=================
		${LAYERS_REV}
	_EOF_
	#######################################################################
	## Set root password to 'root' if 'debug-tweaks' is NOT enabled.
	## command: echo -n 'root' | mkpasswd -5 -s
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
	## for testing purposes (when IMAGE_FEATURES contains 'dey-test')
	#######################################################################
	if echo "${IMAGE_FEATURES}" | grep -qs dey-test; then
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
			rm -f ${IMAGE_ROOTFS}/etc/securetty
		fi
	fi
}
