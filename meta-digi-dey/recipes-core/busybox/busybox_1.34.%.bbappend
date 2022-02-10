# Copyright (C) 2013-2022 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://standby \
            file://standby-actions \
            file://standby-systemd \
            file://busybox-ntpd \
            file://busybox-httpd.service.in \
            file://nm \
            file://busybox-acpid \
            file://acpid.map \
            file://pswitch-standby \
            file://pswitch-poweroff \
            file://bridgeifupdown \
            ${@bb.utils.contains("MACHINE_FEATURES", "mca", "file://poweroff_safe \
                                                             file://reboot_safe \
                                                            ", "", d)} \
           "

SRC_URI_append_ccimx6ul = " file://index.html \
                            file://digi-logo.png \
                          "

HAS_SYSTEMD = "${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}"
HAS_MCA = "${@bb.utils.contains('MACHINE_FEATURES', 'mca', 'true', 'false', d)}"

# hwclock bootscript init parameters
INITSCRIPT_PARAMS_${PN}-hwclock = "start 20 S . stop 20 0 6 ."

FILES_${PN}_append = " ${systemd_unitdir}/system-sleep/"

# HTTPD package
FILES_${PN}-httpd_append = " ${systemd_unitdir}/system/busybox-httpd.service"
SYSTEMD_PACKAGES += "${PN}-httpd"
SYSTEMD_SERVICE_${PN}-httpd = "busybox-httpd.service"

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
		install -d ${D}/srv/www/cgi-bin
		install -m 0755 ${WORKDIR}/nm ${D}/srv/www/cgi-bin/
		if ${HAS_SYSTEMD}; then
			install -d ${D}${systemd_unitdir}/system
			sed 's,@sbindir@,${sbindir},g' < ${WORKDIR}/busybox-httpd.service.in \
				> ${D}${systemd_unitdir}/system/busybox-httpd.service
		fi
	fi
	# Install one standby script or another depending on having systemd or not
	if ${HAS_SYSTEMD}; then
		# Install systemd version of 'standby' script
		install -m 0755 ${WORKDIR}/standby-systemd ${D}${base_bindir}/standby
		# Install systemd 'standby-actions' hook
		install -d ${D}${systemd_unitdir}/system-sleep/
		install -m 0755 ${WORKDIR}/standby-actions ${D}${systemd_unitdir}/system-sleep/
	else
		# Install 'standby' script
		install -m 0755 ${WORKDIR}/standby ${D}${base_bindir}
	fi
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

	# install MCA power safe and reboot safe scripts
	if ${HAS_MCA}; then
		install -m 0755 ${WORKDIR}/poweroff_safe ${D}${base_bindir}/poweroff_safe
		install -m 0755 ${WORKDIR}/reboot_safe ${D}${base_bindir}/reboot_safe
	fi
}

do_install_append_ccimx6ul() {
	install -d ${D}/srv/www
	if grep "CONFIG_HTTPD=y" ${WORKDIR}/defconfig; then
		install -m 0644 ${WORKDIR}/index.html ${D}/srv/www/
		install -m 0644 ${WORKDIR}/digi-logo.png ${D}/srv/www/

	fi
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
