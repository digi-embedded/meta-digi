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
}
