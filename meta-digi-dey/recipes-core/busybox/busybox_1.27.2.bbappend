# Copyright (C) 2013-2018 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BP}:"

SRC_URI += "file://standby \
            file://busybox-ntpd \
            file://index.html \
            file://digi-logo.png \
            file://busybox-acpid \
            file://acpid.map \
            file://pswitch-standby \
            file://pswitch-poweroff \
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
                       ${sysconfdir}/acpi/pswitch-standby \
                       ${sysconfdir}/acpi/pswitch-poweroff \
"
INITSCRIPT_PACKAGES =+ "${PN}-acpid"
INITSCRIPT_NAME_${PN}-acpid = "busybox-acpid"

# Start busybox-syslog at a very early state
INITSCRIPT_PARAMS_${PN}-syslog = "defaults 02"

do_install_append() {
	if grep "CONFIG_NTPD=y" ${WORKDIR}/defconfig; then
		install -m 0755 ${WORKDIR}/busybox-ntpd ${D}${sysconfdir}/init.d/
	fi
	if grep "CONFIG_HTTPD=y" ${WORKDIR}/defconfig; then
		install -m 0644 ${WORKDIR}/index.html ${D}/srv/www/
		install -m 0644 ${WORKDIR}/digi-logo.png ${D}/srv/www/
	fi
	# Install 'standby' script
	install -m 0755 ${WORKDIR}/standby ${D}${base_bindir}
	# Create a symlink called suspend to maintain backward compatibility
	ln -s standby ${D}${base_bindir}/suspend
	if grep "CONFIG_ACPID=y" ${WORKDIR}/defconfig; then
		install -m 0755 ${WORKDIR}/busybox-acpid ${D}${sysconfdir}/init.d/
		install -d ${D}${sysconfdir}/acpi/
		install -m 0755 ${WORKDIR}/acpid.map ${D}${sysconfdir}/acpi/
		install -m 0755 ${WORKDIR}/pswitch-standby ${D}${sysconfdir}/acpi/
		install -m 0755 ${WORKDIR}/pswitch-poweroff ${D}${sysconfdir}/acpi/
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
