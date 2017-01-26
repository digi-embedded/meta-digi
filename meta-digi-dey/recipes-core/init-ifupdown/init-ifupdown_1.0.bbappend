# Copyright (C) 2013 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BP}:"

INITSCRIPT_NAME = "networking"
INITSCRIPT_PARAMS = "start 03 2 3 4 5 . stop 80 0 6 1 ."

SRC_URI_append = " \
    file://interfaces.br0.example \
    file://interfaces.eth0.static \
    file://interfaces.eth0.dhcp \
    file://interfaces.eth1.static \
    file://interfaces.eth1.dhcp \
    file://interfaces.wlan0.static \
    file://interfaces.wlan0.dhcp \
    file://interfaces.p2p \
    file://interfaces.cellular \
    file://resolv \
"

WPA_DRIVER ?= "nl80211"

do_install_append() {
	# Install DNS servers handler
	install -m 0755 ${WORKDIR}/resolv ${D}${sysconfdir}/network/if-up.d/resolv

	# Create 'interfaces' file dynamically
	cat ${WORKDIR}/interfaces.eth0.${ETH0_MODE} >> ${D}${sysconfdir}/network/interfaces
	[ -n "${HAVE_SECOND_ETH}" ] && cat ${WORKDIR}/interfaces.eth1.${ETH1_MODE} >> ${D}${sysconfdir}/network/interfaces

	if [ -n "${HAVE_WIFI}" ]; then
		cat ${WORKDIR}/interfaces.wlan0.${WLAN0_MODE} >> ${D}${sysconfdir}/network/interfaces
		if [ -n "${WLAN_P2P_INTERFACE}" ]; then
			cat ${WORKDIR}/interfaces.p2p >> ${D}${sysconfdir}/network/interfaces
			[ -n "${WLAN_P2P_AUTO}" ] && sed -i -e 's/^#auto ##WLAN_P2P_INTERFACE##/auto ##WLAN_P2P_INTERFACE##/g' ${D}${sysconfdir}/network/interfaces
			sed -i -e 's,##WLAN_P2P_INTERFACE##,${WLAN_P2P_INTERFACE},g' ${D}${sysconfdir}/network/interfaces
		fi
	fi
	cat ${WORKDIR}/interfaces.br0.example >> ${D}${sysconfdir}/network/interfaces

	# Remove config entries if corresponding variable is not defined
	[ -z "${ETH0_STATIC_DNS}" ] && sed -i -e "/##ETH0_STATIC_DNS##/d" ${D}${sysconfdir}/network/interfaces
	[ -z "${ETH0_STATIC_GATEWAY}" ] && sed -i -e "/##ETH0_STATIC_GATEWAY##/d" ${D}${sysconfdir}/network/interfaces
	[ -z "${ETH0_STATIC_IP}" ] && sed -i -e "/##ETH0_STATIC_IP##/d" ${D}${sysconfdir}/network/interfaces
	[ -z "${ETH0_STATIC_NETMASK}" ] && sed -i -e "/##ETH0_STATIC_NETMASK##/d" ${D}${sysconfdir}/network/interfaces
	[ -z "${ETH1_STATIC_DNS}" ] && sed -i -e "/##ETH1_STATIC_DNS##/d" ${D}${sysconfdir}/network/interfaces
	[ -z "${ETH1_STATIC_GATEWAY}" ] && sed -i -e "/##ETH1_STATIC_GATEWAY##/d" ${D}${sysconfdir}/network/interfaces
	[ -z "${ETH1_STATIC_IP}" ] && sed -i -e "/##ETH1_STATIC_IP##/d" ${D}${sysconfdir}/network/interfaces
	[ -z "${ETH1_STATIC_NETMASK}" ] && sed -i -e "/##ETH1_STATIC_NETMASK##/d" ${D}${sysconfdir}/network/interfaces
	[ -z "${WLAN0_STATIC_DNS}" ] && sed -i -e "/##WLAN0_STATIC_DNS##/d" ${D}${sysconfdir}/network/interfaces
	[ -z "${WLAN0_STATIC_GATEWAY}" ] && sed -i -e "/##WLAN0_STATIC_GATEWAY##/d" ${D}${sysconfdir}/network/interfaces
	[ -z "${WLAN0_STATIC_IP}" ] && sed -i -e "/##WLAN0_STATIC_IP##/d" ${D}${sysconfdir}/network/interfaces
	[ -z "${WLAN0_STATIC_NETMASK}" ] && sed -i -e "/##WLAN0_STATIC_NETMASK##/d" ${D}${sysconfdir}/network/interfaces
	[ -z "${P2P0_STATIC_DNS}" ] && sed -i -e "/##P2P0_STATIC_DNS##/d" ${D}${sysconfdir}/network/interfaces
	[ -z "${P2P0_STATIC_GATEWAY}" ] && sed -i -e "/##P2P0_STATIC_GATEWAY##/d" ${D}${sysconfdir}/network/interfaces
	[ -z "${P2P0_STATIC_IP}" ] && sed -i -e "/##P2P0_STATIC_IP##/d" ${D}${sysconfdir}/network/interfaces
	[ -z "${P2P0_STATIC_NETMASK}" ] && sed -i -e "/##P2P0_STATIC_NETMASK##/d" ${D}${sysconfdir}/network/interfaces

	# Cellular interface
	if [ -n "${@bb.utils.contains('DISTRO_FEATURES', 'cellular', '1', '', d)}" ] && [ -n "${CELLULAR_INTERFACE}" ]; then
		cat ${WORKDIR}/interfaces.cellular >> ${D}${sysconfdir}/network/interfaces
		[ -n "${CELLULAR_AUTO}" ] && sed -i -e 's/^#auto ##CELLULAR_INTERFACE##/auto ##CELLULAR_INTERFACE##/g' ${D}${sysconfdir}/network/interfaces
		sed -i -e 's,##CELLULAR_INTERFACE##,${CELLULAR_INTERFACE},g' ${D}${sysconfdir}/network/interfaces

		if [ -n "${CELLULAR_APN}" ]; then
			sed -i -e 's/^\([[:blank:]]*\)apn/\1apn ${CELLULAR_APN}/g' ${D}${sysconfdir}/network/interfaces
		else
			sed -i -e '/^[[:blank:]]*apn/d' ${D}${sysconfdir}/network/interfaces
		fi

		if [ -n "${CELLULAR_PIN}" ]; then
			sed -i -e 's/^\([[:blank:]]*\)pin/\1pin ${CELLULAR_PIN}/g' ${D}${sysconfdir}/network/interfaces
		else
			sed -i -e '/^[[:blank:]]*pin/d' ${D}${sysconfdir}/network/interfaces
		fi

		if [ -n "${CELLULAR_PORT}" ]; then
			sed -i -e 's/^\([[:blank:]]*\)port/\1port ${CELLULAR_PORT}/g' ${D}${sysconfdir}/network/interfaces
			sed -i -e 's,dhcp,manual,g' ${D}${sysconfdir}/network/interfaces
		else
			sed -i -e '/^[[:blank:]]*port/d' ${D}${sysconfdir}/network/interfaces
		fi

		if [ -n "${CELLULAR_USER}" ]; then
			sed -i -e 's/^\([[:blank:]]*\)user/\1user ${CELLULAR_USER}/g' ${D}${sysconfdir}/network/interfaces
		else
			sed -i -e '/^[[:blank:]]*user/d' ${D}${sysconfdir}/network/interfaces
		fi

		if [ -n "${CELLULAR_PASSWORD}" ]; then
			sed -i -e 's/^\([[:blank:]]*\)password/\1password ${CELLULAR_PASSWORD}/g' ${D}${sysconfdir}/network/interfaces
		else
			sed -i -e '/^[[:blank:]]*password/d' ${D}${sysconfdir}/network/interfaces
		fi
	fi

	# Replace interface parameters
	sed -i -e "s,##ETH0_STATIC_IP##,${ETH0_STATIC_IP},g" ${D}${sysconfdir}/network/interfaces
	sed -i -e "s,##ETH0_STATIC_NETMASK##,${ETH0_STATIC_NETMASK},g" ${D}${sysconfdir}/network/interfaces
	sed -i -e "s,##ETH0_STATIC_GATEWAY##,${ETH0_STATIC_GATEWAY},g" ${D}${sysconfdir}/network/interfaces
	sed -i -e "s,##ETH0_STATIC_DNS##,${ETH0_STATIC_DNS},g" ${D}${sysconfdir}/network/interfaces
	sed -i -e "s,##ETH1_STATIC_IP##,${ETH1_STATIC_IP},g" ${D}${sysconfdir}/network/interfaces
	sed -i -e "s,##ETH1_STATIC_NETMASK##,${ETH1_STATIC_NETMASK},g" ${D}${sysconfdir}/network/interfaces
	sed -i -e "s,##ETH1_STATIC_GATEWAY##,${ETH1_STATIC_GATEWAY},g" ${D}${sysconfdir}/network/interfaces
	sed -i -e "s,##ETH1_STATIC_DNS##,${ETH1_STATIC_DNS},g" ${D}${sysconfdir}/network/interfaces
	sed -i -e "s,##WLAN0_STATIC_IP##,${WLAN0_STATIC_IP},g" ${D}${sysconfdir}/network/interfaces
	sed -i -e "s,##WLAN0_STATIC_NETMASK##,${WLAN0_STATIC_NETMASK},g" ${D}${sysconfdir}/network/interfaces
	sed -i -e "s,##WLAN0_STATIC_GATEWAY##,${WLAN0_STATIC_GATEWAY},g" ${D}${sysconfdir}/network/interfaces
	sed -i -e "s,##WLAN0_STATIC_DNS##,${WLAN0_STATIC_DNS},g" ${D}${sysconfdir}/network/interfaces
	sed -i -e "s,##P2P0_STATIC_IP##,${P2P0_STATIC_IP},g" ${D}${sysconfdir}/network/interfaces
	sed -i -e "s,##P2P0_STATIC_NETMASK##,${P2P0_STATIC_NETMASK},g" ${D}${sysconfdir}/network/interfaces
	sed -i -e "s,##P2P0_STATIC_GATEWAY##,${P2P0_STATIC_GATEWAY},g" ${D}${sysconfdir}/network/interfaces
	sed -i -e "s,##P2P0_STATIC_DNS##,${P2P0_STATIC_DNS},g" ${D}${sysconfdir}/network/interfaces
	sed -i -e "s,##WPA_DRIVER##,${WPA_DRIVER},g" ${D}${sysconfdir}/network/interfaces
}
