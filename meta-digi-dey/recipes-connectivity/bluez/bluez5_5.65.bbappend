# Copyright (C) 2015-2023 Digi International.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:${THISDIR}/${BP}:"

SRC_URI += " \
    file://main.conf \
    file://0001-bluetooth.service-add-Digi-customizations.patch \
    file://0002-hcitool-do-not-show-unsupported-refresh-option.patch \
    file://0003-hcitool-increase-the-shown-connection-limit-to-20.patch \
    file://0004-port-test-discovery-to-python3.patch \
    file://0009-bdaddr-support-setting-MAC-address-for-NXP-iw612.patch \
"

QCA65XX_COMMON_PATCHES = " \
    file://0005-Add-hciattach-rome-support-for-Qualcomm-chip-QCA6564.patch \
    file://0006-hciattach_rome-Respect-the-user-indication-for-noflo.patch \
    file://0007-hciattach-If-the-user-supplies-a-bdaddr-use-it.patch \
    file://0008-hciattach-Add-verbosity-option.patch \
"

SRC_URI:append:ccimx6ul = " ${QCA65XX_COMMON_PATCHES}"
SRC_URI:append:ccimx6 = " ${QCA65XX_COMMON_PATCHES}"
SRC_URI:append:ccimx8x = " ${QCA65XX_COMMON_PATCHES}"
SRC_URI:append:ccimx8m = " ${QCA65XX_COMMON_PATCHES}"

SRC_URI:append:ccimx6sbc = " \
    file://main.conf_atheros \
"

do_install:append() {
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

do_install:append:ccimx6sbc() {
	install -m 0644 ${WORKDIR}/main.conf_atheros ${D}${sysconfdir}/bluetooth/
	sed -i -e "s,##BT_DEVICE_NAME##,${BT_DEVICE_NAME},g" \
		${D}${sysconfdir}/bluetooth/main.conf_atheros
}

pkg_postinst_ontarget:${PN}:ccimx6sbc() {
	# Only execute the script on wireless ccimx6 platforms
	if [ -e "/proc/device-tree/bluetooth/mac-address" ]; then
		for id in $(find /sys/devices -name modalias -print0 | xargs -0 sort -u -z | grep sdio); do
			if [[ "$id" == "sdio:c00v0271d0301" ]] ; then
				mv /etc/bluetooth/main.conf_atheros /etc/bluetooth/main.conf
				break
			elif [[ "$id" == "sdio:c00v0271d050A" ]] ; then
				rm /etc/bluetooth/main.conf_atheros
				break
			fi
		done
	fi
}

PACKAGECONFIG:append = " health-profiles \
    mesh \
    btpclient \
"

FILES:${PN} += " ${sysconfdir}/bluetooth/main.conf*"

PACKAGE_ARCH = "${MACHINE_ARCH}"
