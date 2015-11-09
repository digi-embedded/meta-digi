# Copyright (C) 2013 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BP}:"

SRC_URI_append = " \
    file://interfaces.br0.example \
    file://interfaces.eth0.static \
    file://interfaces.eth0.dhcp \
    file://interfaces.eth1.static \
    file://interfaces.eth1.dhcp \
    file://interfaces.wlan0.static \
    file://interfaces.wlan0.dhcp \
    file://resolv \
"

WPA_DRIVER ?= "nl80211"

do_install_append() {
	# Install DNS servers handler
	install -m 0755 ${WORKDIR}/resolv ${D}${sysconfdir}/network/if-up.d/resolv

	# Create 'interfaces' file dynamically
	cat ${WORKDIR}/interfaces.eth0.${ETH0_MODE} >> ${D}${sysconfdir}/network/interfaces
	[ -n "${HAVE_EXT_ETH}" ] && cat ${WORKDIR}/interfaces.eth1.${ETH1_MODE} >> ${D}${sysconfdir}/network/interfaces
	[ -n "${HAVE_WIFI}" ] && cat ${WORKDIR}/interfaces.wlan0.${WLAN0_MODE} >> ${D}${sysconfdir}/network/interfaces
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
	sed -i -e "s,##WPA_DRIVER##,${WPA_DRIVER},g" ${D}${sysconfdir}/network/interfaces
}
