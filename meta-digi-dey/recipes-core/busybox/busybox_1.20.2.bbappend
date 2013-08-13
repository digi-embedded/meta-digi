# Copyright (C) 2013 Digi International.

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}-${PV}:"

DEPENDS += "libdigi"

SRC_URI += "file://0001-del-baudrates.patch \
            file://0002-del-mdev_regulatory.patch \
            file://0003-del-flash_eraseall.patch \
            file://0004-ntpd-indefinitely-try-to-resolve-peer-addresses.patch \
            file://adc \
            file://mmc \
            file://sd \
            file://ts \
            file://suspend \
            file://busybox-ntpd \
            file://index.html \
            file://digi.gif \
            file://busybox-acpid \
            file://acpid.map \
            file://pswitch-press \
            file://pswitch-release \
           "

# Add device handlers to 'mdev' package
FILES_${PN}-mdev += " \
	${base_bindir}/mmc-mount \
	${base_bindir}/mmc-umount \
	${base_bindir}/usbmount \
	${base_bindir}/usbumount \
	${base_libdir}/mdev/adc \
	${base_libdir}/mdev/mmc \
	${base_libdir}/mdev/sd \
	${base_libdir}/mdev/ts \
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

do_install_append() {
	if grep "CONFIG_MDEV=y" ${WORKDIR}/defconfig; then
		if grep "CONFIG_FEATURE_MDEV_CONF=y" ${WORKDIR}/defconfig; then
			install -d ${D}${base_libdir}/mdev
			install -m 0755 ${WORKDIR}/adc ${D}${base_libdir}/mdev/adc
			install -m 0755 ${WORKDIR}/mmc ${D}${base_libdir}/mdev/mmc
			install -m 0755 ${WORKDIR}/sd ${D}${base_libdir}/mdev/sd
			install -m 0755 ${WORKDIR}/ts ${D}${base_libdir}/mdev/ts
			ln -s ../lib/mdev/mmc ${D}${base_bindir}/mmc-mount
			ln -s ../lib/mdev/mmc ${D}${base_bindir}/mmc-umount
			ln -s ../lib/mdev/sd ${D}${base_bindir}/usbmount
			ln -s ../lib/mdev/sd ${D}${base_bindir}/usbumount
		fi
	fi
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
}
