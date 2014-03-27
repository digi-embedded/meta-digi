# Copyright (C) 2013 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

DEPENDS += "libdigi"

SRC_URI += "file://0001-del-baudrates.patch \
            file://0002-del-flash_eraseall.patch \
            file://0003-ntpd-indefinitely-try-to-resolve-peer-addresses.patch \
            file://suspend \
            file://busybox-ntpd \
            file://index.html \
            file://digi.gif \
            file://busybox-acpid \
            file://acpid.map \
            file://pswitch-press \
            file://pswitch-release \
            file://busybox-static-nodes \
           "

# hwclock bootscript init parameters
INITSCRIPT_PARAMS_${PN}-hwclock = "start 20 S . stop 20 0 6 ."

# NTPD package
PACKAGES =+ "${PN}-ntpd"
FILES_${PN}-ntpd = "${sysconfdir}/init.d/busybox-ntpd"
INITSCRIPT_PACKAGES =+ "${PN}-ntpd"
INITSCRIPT_NAME_${PN}-ntpd = "busybox-ntpd"

# ACPID package
PACKAGES =+ "${PN}-acpid"
FILES_${PN}-acpid = " ${sysconfdir}/init.d/busybox-acpid \
                       ${sysconfdir}/acpi/acpid.map \
                       ${sysconfdir}/acpi/pswitch-press \
                       ${sysconfdir}/acpi/pswitch-release \
"
INITSCRIPT_PACKAGES =+ "${PN}-acpid"
INITSCRIPT_NAME_${PN}-acpid = "busybox-acpid"

# static-nodes package (create static nodes from /etc/device_table)
PACKAGES =+ "${PN}-static-nodes"
FILES_${PN}-static-nodes = "${sysconfdir}/init.d/busybox-static-nodes"
INITSCRIPT_PACKAGES =+ "${PN}-static-nodes"
INITSCRIPT_NAME_${PN}-static-nodes = "busybox-static-nodes"
INITSCRIPT_PARAMS_${PN}-static-nodes = "start 07 S ."

do_install_append() {
	if grep "CONFIG_NTPD=y" ${WORKDIR}/defconfig; then
		install -m 0755 ${WORKDIR}/busybox-ntpd ${D}${sysconfdir}/init.d/
	fi
	if grep "CONFIG_HTTPD=y" ${WORKDIR}/defconfig; then
		install -m 0644 ${WORKDIR}/index.html ${D}/srv/www/
		install -m 0644 ${WORKDIR}/digi.gif ${D}/srv/www/
	fi
	# Install 'suspend' script
	install -m 0755 ${WORKDIR}/suspend ${D}${base_bindir}
	if grep "CONFIG_ACPID=y" ${WORKDIR}/defconfig; then
		install -m 0755 ${WORKDIR}/busybox-acpid ${D}${sysconfdir}/init.d/
		install -d ${D}${sysconfdir}/acpi/
		install -m 0755 ${WORKDIR}/acpid.map ${D}${sysconfdir}/acpi/
		install -m 0755 ${WORKDIR}/pswitch-press ${D}${sysconfdir}/acpi/
		install -m 0755 ${WORKDIR}/pswitch-release ${D}${sysconfdir}/acpi/
	fi
	if grep "CONFIG_MAKEDEVS=y" ${WORKDIR}/defconfig; then
		install -m 0755 ${WORKDIR}/busybox-static-nodes ${D}${sysconfdir}/init.d/
	fi
}
