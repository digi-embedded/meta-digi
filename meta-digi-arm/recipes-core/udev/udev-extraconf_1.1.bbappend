# Copyright (C) 2013-2022 Digi International.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI += " \
    file://mount_digiparts.sh \
    file://81-spi-spidev.rules \
    file://blacklist.conf \
"

do_install:append() {

	install -d ${D}/mnt
	install -d ${D}/mnt/linux
	install -d ${D}/mnt/update
	install -d ${D}/mnt/data

	install -m 0755 ${WORKDIR}/mount_digiparts.sh ${D}${sysconfdir}/udev/scripts/
        sed -i -e 's|@base_sbindir@|${base_sbindir}|g' ${D}${sysconfdir}/udev/scripts/mount_digiparts.sh
        sed -i -e 's|@systemd_unitdir@|${systemd_unitdir}|g' ${D}${sysconfdir}/udev/scripts/mount_digiparts.sh
	install -m 0644 ${WORKDIR}/81-spi-spidev.rules ${D}${sysconfdir}/udev/rules.d/

	# Bluetooth tty symlink
	if [ -n "${BT_TTY}" ]; then
		printf "%s\n%s\n" \
		       "# Symlink to the bluetooth tty" \
		       "KERNEL==\"${BT_TTY}\", MODE=\"0660\", GROUP=\"dialout\", SYMLINK+=\"ttyBt\"" \
		       >> ${D}${sysconfdir}/udev/rules.d/localextra.rules
	fi

	# XBee TTY symlink
	if [ -n "${XBEE_TTY}" ]; then
		printf "%s\n%s\n" \
		       "# Symlink to the XBee tty" \
		       "KERNEL==\"${XBEE_TTY}\", MODE=\"0660\", GROUP=\"tty\", SYMLINK+=\"ttyXBee\"" \
		       >> ${D}${sysconfdir}/udev/rules.d/localextra.rules
	fi

	# Mouse symlink
	printf "%s\n%s\n" \
	       "# Symlink to the mouse" \
	       "SUBSYSTEM==\"input\", KERNEL==\"event[0-9]*\", ENV{ID_INPUT_MOUSE}==\"1\", SYMLINK+=\"input/mouse0\"" \
	       >> ${D}${sysconfdir}/udev/rules.d/localextra.rules

	install -d ${D}${sysconfdir}/modprobe.d
	if ${@bb.utils.contains('MACHINE_FEATURES','gpu','false','true',d)}; then
		# evbug debug tool
		install -m 0644 ${WORKDIR}/blacklist.conf ${D}${sysconfdir}/modprobe.d
	fi
}

FILES:${PN}:append = " ${sysconfdir}/modprobe.d"
FILES:${PN}:append = " /mnt"

# BT_TTY is machine specific (defined in machine config file)
PACKAGE_ARCH = "${MACHINE_ARCH}"
