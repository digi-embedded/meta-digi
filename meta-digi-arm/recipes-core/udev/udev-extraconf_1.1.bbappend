# Copyright (C) 2013-2017 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI += " \
    file://mount_bootparts.sh \
    file://mount_partition.sh \
    file://81-spi-spidev.rules \
"

do_install_append() {
	install -m 0755 ${WORKDIR}/mount_bootparts.sh ${D}${sysconfdir}/udev/scripts/
	install -m 0755 ${WORKDIR}/mount_partition.sh ${D}${sysconfdir}/udev/scripts/
	install -m 0644 ${WORKDIR}/81-spi-spidev.rules ${D}${sysconfdir}/udev/rules.d/

	# Bluetooth tty symlink
	if [ -n "${BT_TTY}" ]; then
		printf "%s\n%s\n" \
		       "# Symlink to the bluetooth tty" \
		       "KERNEL==\"${BT_TTY}\", MODE=\"0660\", GROUP=\"dialout\", SYMLINK+=\"ttyBt\"" \
		       >> ${D}${sysconfdir}/udev/rules.d/localextra.rules
	fi
}

# BT_TTY is machine specific (defined in machine config file)
PACKAGE_ARCH = "${MACHINE_ARCH}"
