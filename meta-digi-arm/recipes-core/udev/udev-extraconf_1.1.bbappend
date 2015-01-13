# Copyright (C) 2013 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://mount_bootparts.sh"

do_install_append() {
	install -m 0755 ${WORKDIR}/mount_bootparts.sh ${D}${sysconfdir}/udev/scripts/

	# Console tty symlink
	if [ -n "${CON_TTY}" ]; then
		printf "%s\n%s\n" \
		       "# Symlink to the console tty" \
		       "KERNEL==\"${CON_TTY}\", MODE=\"0660\", GROUP=\"dialout\", SYMLINK+=\"ttyS0\"" \
		       >> ${D}${sysconfdir}/udev/rules.d/localextra.rules
	fi

	# Bluetooth tty symlink
	if [ -n "${BT_TTY}" ]; then
		printf "%s\n%s\n" \
		       "# Symlink to the bluetooth tty" \
		       "KERNEL==\"${BT_TTY}\", MODE=\"0660\", GROUP=\"dialout\", SYMLINK+=\"ttyBt\"" \
		       >> ${D}${sysconfdir}/udev/rules.d/localextra.rules
	fi
}

# CON_TTY and BT_TTY depend on linux version so make it a signature dependence
do_install[vardeps] += "PREFERRED_VERSION_linux-dey"

# BT_TTY is machine specific (defined in machine config file)
PACKAGE_ARCH = "${MACHINE_ARCH}"
