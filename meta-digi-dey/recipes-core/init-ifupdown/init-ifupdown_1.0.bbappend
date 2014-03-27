# Copyright (C) 2013 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}-${PV}:"

SRC_URI_append_mx5 = " file://ifup"

WPA_DRIVER ?= "wext"

do_install_append() {
	# Enable or disable second ethernet interface
	if [ -n "${HAVE_EXT_ETH}" ]; then
		sed -i -e '/^.*auto eth1.*/cauto eth1' ${D}${sysconfdir}/network/interfaces
	else
		sed -i -e '/^.*auto eth1.*/c#auto eth1' ${D}${sysconfdir}/network/interfaces
	fi
	# Enable or disable wifi interface
	if [ -n "${HAVE_WIFI}" ]; then
		sed -i -e '/^.*auto wlan0.*/cauto wlan0' ${D}${sysconfdir}/network/interfaces
	else
		sed -i -e '/^.*auto wlan0.*/c#auto wlan0' ${D}${sysconfdir}/network/interfaces
	fi
	# Configure wpa_supplicant driver
	sed -i -e "s,##WPA_DRIVER##,${WPA_DRIVER},g" ${D}${sysconfdir}/network/interfaces
}

do_install_append_mx5() {
	install -m 0755 ${WORKDIR}/ifup ${D}${sysconfdir}/network/if-up.d
}
