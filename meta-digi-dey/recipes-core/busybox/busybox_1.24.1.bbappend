# Copyright (C) 2013 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BP}:"

SRC_URI += "file://0001-del-baudrates.patch \
            file://0002-ntpd-indefinitely-try-to-resolve-peer-addresses.patch \
            file://suspend \
            file://busybox-ntpd \
            file://index.html \
            file://digi-logo.png \
            file://busybox-acpid \
            file://acpid.map \
            file://pswitch-suspend \
            file://pswitch-poweroff \
            file://busybox-static-nodes \
            file://bridgeifupdown \
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
                       ${sysconfdir}/acpi/pswitch-suspend \
                       ${sysconfdir}/acpi/pswitch-poweroff \
"
INITSCRIPT_PACKAGES =+ "${PN}-acpid"
INITSCRIPT_NAME_${PN}-acpid = "busybox-acpid"

# Start busybox-syslog at a very early state
INITSCRIPT_PARAMS_${PN}-syslog = "defaults 02"

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
		install -m 0644 ${WORKDIR}/digi-logo.png ${D}/srv/www/
	fi
	# Install 'suspend' script
	install -m 0755 ${WORKDIR}/suspend ${D}${base_bindir}
	if grep "CONFIG_ACPID=y" ${WORKDIR}/defconfig; then
		install -m 0755 ${WORKDIR}/busybox-acpid ${D}${sysconfdir}/init.d/
		install -d ${D}${sysconfdir}/acpi/
		install -m 0755 ${WORKDIR}/acpid.map ${D}${sysconfdir}/acpi/
		install -m 0755 ${WORKDIR}/pswitch-suspend ${D}${sysconfdir}/acpi/
		install -m 0755 ${WORKDIR}/pswitch-poweroff ${D}${sysconfdir}/acpi/
	fi
	if grep "CONFIG_MAKEDEVS=y" ${WORKDIR}/defconfig; then
		install -m 0755 ${WORKDIR}/busybox-static-nodes ${D}${sysconfdir}/init.d/
	fi

	# Install bridgeifupdown script
	if grep "CONFIG_BRCTL" ${WORKDIR}/defconfig; then
		install -d ${D}${sysconfdir}/network/if-pre-up.d/
		install -d ${D}${sysconfdir}/network/if-post-down.d/
		install -m 0755 ${WORKDIR}/bridgeifupdown ${D}${sysconfdir}/network/if-pre-up.d/
		ln -s ../if-pre-up.d/bridgeifupdown ${D}${sysconfdir}/network/if-post-down.d/bridgeifupdown
	fi
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
