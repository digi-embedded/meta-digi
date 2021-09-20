# Copyright (C) 2015-2021 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:${THISDIR}/${BP}:"

SRC_URI += " \
    file://bluetooth-init \
    file://bluetooth-init.service \
    file://bluetooth.service-add-customizations.patch \
    file://main.conf \
    file://0001-hcitool-do-not-show-unsupported-refresh-option.patch \
    file://0002-hcitool-increase-the-shown-connection-limit-to-20.patch \
    file://0003-port-test-discovery-to-python3.patch \
    file://0008-tools-Use-l_main_run_with_signal-instead-of-open-cod.patch \
"

QCA65XX_COMMON_PATCHES = " \
    file://0004-QCA_bluetooth_chip_support.patch \
    file://0005-hciattach_rome-Respect-the-user-indication-for-noflo.patch \
    file://0006-hciattach-If-the-user-supplies-a-bdaddr-use-it.patch \
    file://0007-hciattach-Add-verbosity-option.patch \
"

SRC_URI_append_ccimx6ul = " ${QCA65XX_COMMON_PATCHES}"
SRC_URI_append_ccimx6 = " ${QCA65XX_COMMON_PATCHES}"
SRC_URI_append_ccimx8x = " ${QCA65XX_COMMON_PATCHES}"
SRC_URI_append_ccimx8m = " ${QCA65XX_COMMON_PATCHES}"

SRC_URI_append_ccimx6sbc = " \
    file://bluetooth-init_atheros \
    file://main.conf_atheros \
"

inherit update-rc.d

do_install_append() {
	install -d ${D}${sysconfdir}/init.d/
	install -m 0755 ${WORKDIR}/bluetooth-init ${D}${sysconfdir}/bluetooth-init
	ln -sf /etc/bluetooth-init ${D}${sysconfdir}/init.d/bluetooth-init
	install -d ${D}${systemd_unitdir}/system/
	install -m 0644 ${WORKDIR}/bluetooth-init.service ${D}${systemd_unitdir}/system/bluetooth-init.service
	install -m 0644 ${WORKDIR}/main.conf ${D}${sysconfdir}/bluetooth/
	sed -i -e "s,##BT_DEVICE_NAME##,${BT_DEVICE_NAME},g" \
		${D}${sysconfdir}/bluetooth/main.conf

	# Staging bluetooth internal headers and libs to allow other recipes
	# to link against them
	install -d ${D}${includedir}/bluetooth-internal/
	install -m 0644 ${WORKDIR}/bluez-${PV}/src/shared/*.h ${D}${includedir}/bluetooth-internal/
	install -m 0644 ${WORKDIR}/bluez-${PV}/lib/uuid.h ${D}${includedir}/bluetooth-internal/
	install -m 0644 ${B}/lib/.libs/libbluetooth-internal.a ${D}${libdir}/
	install -m 0644 ${B}/src/.libs/libshared-mainloop.a ${D}${libdir}/
	# Fix include path for att-types.h
	sed -i -e '/#include/{s,src/shared/,,g}' ${D}${includedir}/bluetooth-internal/att.h
}

do_install_append_ccimx6sbc() {
	install -m 0755 ${WORKDIR}/bluetooth-init_atheros ${D}${sysconfdir}/bluetooth-init_atheros
	install -m 0644 ${WORKDIR}/main.conf_atheros ${D}${sysconfdir}/bluetooth/
	sed -i -e "s,##BT_DEVICE_NAME##,${BT_DEVICE_NAME},g" \
		${D}${sysconfdir}/bluetooth/main.conf_atheros
}

pkg_postinst_ontarget_${PN}_ccimx6sbc() {
	# Only execute the script on wireless ccimx6 platforms
	if [ -e "/proc/device-tree/bluetooth/mac-address" ]; then
		for id in $(find /sys/devices -name modalias -print0 | xargs -0 sort -u -z | grep sdio); do
			if [[ "$id" == "sdio:c00v0271d0301" ]] ; then
				mv /etc/bluetooth-init_atheros /etc/bluetooth-init
				mv /etc/bluetooth/main.conf_atheros /etc/bluetooth/main.conf
				break
			elif [[ "$id" == "sdio:c00v0271d050A" ]] ; then
				rm /etc/bluetooth-init_atheros
				rm /etc/bluetooth/main.conf_atheros
				break
			fi
		done
	fi
}

PACKAGES =+ "${PN}-init"
PACKAGECONFIG_append = " health-profiles \
    mesh \
    btpclient \
"

FILES_${PN} += " ${sysconfdir}/bluetooth/main.conf*"
FILES_${PN}-init = " ${sysconfdir}/bluetooth-init* \
                     ${sysconfdir}/init.d/bluetooth-init \
                     ${systemd_unitdir}/system/bluetooth-init.service \
"

INITSCRIPT_PACKAGES += "${PN}-init"
INITSCRIPT_NAME_${PN}-init = "bluetooth-init"
INITSCRIPT_PARAMS_${PN}-init = "start 19 2 3 4 5 . stop 21 0 1 6 ."

SYSTEMD_SERVICE_${PN}-init = "bluetooth-init.service"

PACKAGE_ARCH = "${MACHINE_ARCH}"
