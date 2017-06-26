# Copyright (C) 2013-2017 Digi International Inc.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BP}:"

INITSCRIPT_NAME = "networking"
INITSCRIPT_PARAMS = "start 03 2 3 4 5 . stop 80 0 6 1 ."

SRC_URI_append = " \
    file://interfaces.br0.example \
    file://interfaces.p2p \
    file://resolv \
"

SRC_URI_append_ccimx6ul = "\
    file://interfaces.wlan1.static \
    file://interfaces.wlan1.dhcp \
    file://virtwlans.sh \
"

WPA_DRIVER ?= "nl80211"

do_install_append() {
	# Install DNS servers handler
	install -m 0755 ${WORKDIR}/resolv ${D}${sysconfdir}/network/if-up.d/resolv

	if [ -n "${HAVE_WIFI}" ]; then
		if [ -n "${WLAN_P2P_INTERFACE}" ]; then
			cat ${WORKDIR}/interfaces.p2p >> ${D}${sysconfdir}/network/interfaces
			[ -n "${WLAN_P2P_AUTO}" ] && sed -i -e 's/^#auto ##WLAN_P2P_INTERFACE##/auto ##WLAN_P2P_INTERFACE##/g' ${D}${sysconfdir}/network/interfaces
			sed -i -e 's,##WLAN_P2P_INTERFACE##,${WLAN_P2P_INTERFACE},g' ${D}${sysconfdir}/network/interfaces
		fi
	fi
	cat ${WORKDIR}/interfaces.br0.example >> ${D}${sysconfdir}/network/interfaces

	# Remove config entries if corresponding variable is not defined
	[ -z "${P2P0_STATIC_DNS}" ] && sed -i -e "/##P2P0_STATIC_DNS##/d" ${D}${sysconfdir}/network/interfaces
	[ -z "${P2P0_STATIC_GATEWAY}" ] && sed -i -e "/##P2P0_STATIC_GATEWAY##/d" ${D}${sysconfdir}/network/interfaces
	[ -z "${P2P0_STATIC_IP}" ] && sed -i -e "/##P2P0_STATIC_IP##/d" ${D}${sysconfdir}/network/interfaces
	[ -z "${P2P0_STATIC_NETMASK}" ] && sed -i -e "/##P2P0_STATIC_NETMASK##/d" ${D}${sysconfdir}/network/interfaces

	# Replace interface parameters
	sed -i -e "s,##P2P0_STATIC_IP##,${P2P0_STATIC_IP},g" ${D}${sysconfdir}/network/interfaces
	sed -i -e "s,##P2P0_STATIC_NETMASK##,${P2P0_STATIC_NETMASK},g" ${D}${sysconfdir}/network/interfaces
	sed -i -e "s,##P2P0_STATIC_GATEWAY##,${P2P0_STATIC_GATEWAY},g" ${D}${sysconfdir}/network/interfaces
	sed -i -e "s,##P2P0_STATIC_DNS##,${P2P0_STATIC_DNS},g" ${D}${sysconfdir}/network/interfaces
	sed -i -e "s,##WPA_DRIVER##,${WPA_DRIVER},g" ${D}${sysconfdir}/network/interfaces
}

do_install_append_ccimx6ul() {
	install -d ${D}${base_bindir}
	install -m 0755 ${WORKDIR}/virtwlans.sh ${D}${base_bindir}
	if [ -n "${HAVE_WIFI}" ]; then
		cat ${WORKDIR}/interfaces.wlan1.${WLAN1_MODE} >> ${D}${sysconfdir}/network/interfaces
	fi

	# Remove config entries if corresponding variable is not defined
	[ -z "${WLAN1_STATIC_DNS}" ] && sed -i -e "/##WLAN1_STATIC_DNS##/d" ${D}${sysconfdir}/network/interfaces
	[ -z "${WLAN1_STATIC_GATEWAY}" ] && sed -i -e "/##WLAN1_STATIC_GATEWAY##/d" ${D}${sysconfdir}/network/interfaces
	[ -z "${WLAN1_STATIC_IP}" ] && sed -i -e "/##WLAN1_STATIC_IP##/d" ${D}${sysconfdir}/network/interfaces
	[ -z "${WLAN1_STATIC_NETMASK}" ] && sed -i -e "/##WLAN1_STATIC_NETMASK##/d" ${D}${sysconfdir}/network/interfaces

	# Replace interface parameters
	sed -i -e "s,##WLAN1_STATIC_IP##,${WLAN1_STATIC_IP},g" ${D}${sysconfdir}/network/interfaces
	sed -i -e "s,##WLAN1_STATIC_NETMASK##,${WLAN1_STATIC_NETMASK},g" ${D}${sysconfdir}/network/interfaces
	sed -i -e "s,##WLAN1_STATIC_GATEWAY##,${WLAN1_STATIC_GATEWAY},g" ${D}${sysconfdir}/network/interfaces
	sed -i -e "s,##WLAN1_STATIC_DNS##,${WLAN1_STATIC_DNS},g" ${D}${sysconfdir}/network/interfaces
}

# Disable wireless interfaces on first boot for non-wireless variants
pkg_postinst_${PN}() {
	if [ -n "$D" ]; then
		exit 1
	fi

	if [ ! -d "/proc/device-tree/wireless" ]; then
		sed -i -e '/^auto wlan/{s,^,#,g};/^auto p2p/{s,^,#,g}' /etc/network/interfaces
	fi

	# Create the symlinks in the different runlevels
	if type update-rc.d >/dev/null 2>/dev/null; then
		update-rc.d ${INITSCRIPT_NAME} ${INITSCRIPT_PARAMS}
	fi

	exit 0
}
