# Copyright (C) 2013-2025, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI += " \
    file://mount_digiparts.sh \
    file://81-spi-spidev.rules \
    file://blacklist.conf \
"
SRC_URI:append:ccmp1 = " \
    file://99-ext-rtc-wakeup.rules \
"

WLAN_AP_INTERFACE = "wlan1"
WLAN_AP_INTERFACE:ccimx9 = "uap0"

do_install:append() {
	if [ -n "${@bb.utils.contains('IMAGE_FEATURES', 'read-only-rootfs', '1', '', d)}" ]; then
		install -d ${D}/mnt
		install -d ${D}/mnt/linux
		install -d ${D}/mnt/update
		install -d ${D}/mnt/data
		# Change mount.sh to check "/sbin/init.orig" additionally to determine if a system is using systemd.
		sed -i '/^BASE_INIT=/a BASE_INIT_ORIG="$(readlink -f "@base_sbindir@/init.orig")"' \
		${D}${sysconfdir}/udev/scripts/mount.sh
		sed -i 's/if \[ "x$BASE_INIT" = "x$INIT_SYSTEMD" \];then/if \[ "x$BASE_INIT" = "x$INIT_SYSTEMD" \] || \[ "x$BASE_INIT_ORIG" = "x$INIT_SYSTEMD" \]; then/' \
		${D}${sysconfdir}/udev/scripts/mount.sh
		sed -i -e 's|@base_sbindir@|${base_sbindir}|g' ${D}${sysconfdir}/udev/scripts/mount.sh
	fi

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

	install -d ${D}${sysconfdir}/modprobe.d
	if ${@bb.utils.contains('MACHINE_FEATURES','gpu','false','true',d)}; then
		# evbug debug tool
		install -m 0644 ${WORKDIR}/blacklist.conf ${D}${sysconfdir}/modprobe.d
	fi

	# Fix mount.sh to force to find files in /tmp as symlink
	sed -i -e 's|find /tmp|find -L /tmp|g' ${D}${sysconfdir}/udev/scripts/mount.sh

	# When a WiFi driver adds AP or P2P (or bridge with AP), it generates udev triggers that lead the wifi card to fail.
	# The grep argument is the list of ifaces to skip, in this case the AP, P2P, and bridge interfaces.
	sed -i -e 's|grep -q wifi|grep -qE "${WLAN_AP_INTERFACE}\|${WLAN_P2P_INTERFACE}\|br0"|g' ${D}${sysconfdir}/udev/scripts/network.sh
}

do_install:append:ccmp1() {
	install -m 0644 ${WORKDIR}/99-ext-rtc-wakeup.rules ${D}${sysconfdir}/udev/rules.d/
}

FILES:${PN}:append = " \
    ${sysconfdir}/modprobe.d \
    ${@bb.utils.contains('IMAGE_FEATURES', 'read-only-rootfs', ' /mnt', '', d)} \
"

# BT_TTY is machine specific (defined in machine config file)
PACKAGE_ARCH = "${MACHINE_ARCH}"
