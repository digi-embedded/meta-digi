# Copyright (C) 2017-2019, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI += " \
    file://0001-networkmanager-trigger-dispatcher-on-per-device-conn.patch \
    file://0002-connectivity-add-config-option-for-response-timeout.patch \
    file://NetworkManager.conf \
    file://networkmanager-init \
    file://nm.cellular \
    file://nm.eth0.dhcp \
    file://nm.eth0.static \
    file://nm.eth1.dhcp \
    file://nm.eth1.static \
    file://nm.wlan0.dhcp \
    file://nm.wlan0.static \
    file://01dispatcher \
    file://ifdownup \
    file://p2pbridge \
"

#
# Adjust compile time options:
#   * consolekit depends on X11. Disable to allow building framebuffer images.
#
PACKAGECONFIG:remove:dey = "consolekit nss"
PACKAGECONFIG:append = " gnutls modemmanager ppp concheck"

#
# NetworkManager only accepts IP addresses in CIDR format
#
def ipaddr_to_cidr(iface, d):
    ipaddr = d.getVar('%s_STATIC_IP' % iface.upper(), True)
    netmask = d.getVar('%s_STATIC_NETMASK' % iface.upper(), True)
    binary_str = ''
    for byte in netmask.split('.'):
        binary_str += bin(int(byte))[2:].zfill(8)
    return ipaddr + '/' + str(len(binary_str.rstrip('0')))

ETH0_STATIC_CIDR = "${@ipaddr_to_cidr('eth0', d)}"
ETH1_STATIC_CIDR = "${@ipaddr_to_cidr('eth1', d)}"
WLAN0_STATIC_CIDR = "${@ipaddr_to_cidr('wlan0', d)}"

inherit update-rc.d

do_install:append() {
	install -d ${D}${sysconfdir}/init.d ${D}${sysconfdir}/NetworkManager
	install -m 0644 ${WORKDIR}/NetworkManager.conf ${D}${sysconfdir}/NetworkManager/
	install -m 0755 ${WORKDIR}/networkmanager-init ${D}${sysconfdir}/init.d/networkmanager

	#
	# Connections config files
	#
	install -d ${D}${sysconfdir}/NetworkManager/system-connections/

	# Ethernet
	install -m 0600 ${WORKDIR}/nm.eth0.${ETH0_MODE} ${D}${sysconfdir}/NetworkManager/system-connections/nm.eth0
	sed -i  -e "s,##ETH0_STATIC_CIDR##,${ETH0_STATIC_CIDR},g" \
		-e "s,##ETH0_STATIC_GATEWAY##,${ETH0_STATIC_GATEWAY},g" \
		-e "s,##ETH0_STATIC_DNS##,${ETH0_STATIC_DNS},g" \
		${D}${sysconfdir}/NetworkManager/system-connections/nm.eth0

	# Ethernet (second interface)
	install -m 0600 ${WORKDIR}/nm.eth1.${ETH1_MODE} ${D}${sysconfdir}/NetworkManager/system-connections/nm.eth1
	sed -i  -e "s,##ETH1_STATIC_CIDR##,${ETH1_STATIC_CIDR},g" \
		-e "s,##ETH1_STATIC_GATEWAY##,${ETH1_STATIC_GATEWAY},g" \
		-e "s,##ETH1_STATIC_DNS##,${ETH1_STATIC_DNS},g" \
		${D}${sysconfdir}/NetworkManager/system-connections/nm.eth1

	# Wireless (only IP settings; connection settings need to be provided at runtime)
	install -m 0600 ${WORKDIR}/nm.wlan0.${WLAN0_MODE} ${D}${sysconfdir}/NetworkManager/system-connections/nm.wlan0
	sed -i  -e "s,##WLAN0_STATIC_CIDR##,${WLAN0_STATIC_CIDR},g" \
		-e "s,##WLAN0_STATIC_GATEWAY##,${WLAN0_STATIC_GATEWAY},g" \
		-e "s,##WLAN0_STATIC_DNS##,${WLAN0_STATIC_DNS},g" \
		${D}${sysconfdir}/NetworkManager/system-connections/nm.wlan0

	# Cellular
	if [ -n "${@bb.utils.contains('DISTRO_FEATURES', 'cellular', '1', '', d)}" ]; then
		install -m 0600 ${WORKDIR}/nm.cellular ${D}${sysconfdir}/NetworkManager/system-connections/nm.cellular
		[ -z "${CELLULAR_APN}" ] && sed -i -e "/##CELLULAR_APN##/d" ${D}${sysconfdir}/NetworkManager/system-connections/nm.cellular
		[ -z "${CELLULAR_PIN}" ] && sed -i -e "/##CELLULAR_PIN##/d" ${D}${sysconfdir}/NetworkManager/system-connections/nm.cellular
		[ -z "${CELLULAR_USER}" ] && sed -i -e "/##CELLULAR_USER##/d" ${D}${sysconfdir}/NetworkManager/system-connections/nm.cellular
		[ -z "${CELLULAR_PASSWORD}" ] && sed -i -e "/##CELLULAR_PASSWORD##/d" ${D}${sysconfdir}/NetworkManager/system-connections/nm.cellular
		sed -i  -e "s,##CELLULAR_APN##,${CELLULAR_APN},g" \
			-e "s,##CELLULAR_PIN##,${CELLULAR_PIN},g" \
			-e "s,##CELLULAR_USER##,${CELLULAR_USER},g" \
			-e "s,##CELLULAR_PASSWORD##,${CELLULAR_PASSWORD},g" \
			${D}${sysconfdir}/NetworkManager/system-connections/nm.cellular
	fi

	# Install dispatcher scripts and create directories
	install -d ${D}${sysconfdir}/NetworkManager/dispatcher.d/up.d \
		${D}${sysconfdir}/NetworkManager/dispatcher.d/down.d \
		${D}${sysconfdir}/NetworkManager/dispatcher.d/connectivity-change.d \
		${D}${sysconfdir}/NetworkManager/dispatcher.d/device-connectivity-change.d
	install -m 0755 ${WORKDIR}/01dispatcher ${D}${sysconfdir}/NetworkManager/dispatcher.d/
	install -m 0755 ${WORKDIR}/ifdownup ${D}${sysconfdir}/NetworkManager/dispatcher.d/device-connectivity-change.d/
	install -m 0755 ${WORKDIR}/p2pbridge ${D}${sysconfdir}/NetworkManager/dispatcher.d/pre-up.d/
	ln -s ../pre-up.d/p2pbridge ${D}${sysconfdir}/NetworkManager/dispatcher.d/down.d/p2pbridge

	# Disable terminal colors by default
	install -d ${D}${sysconfdir}/terminal-colors.d
	touch ${D}${sysconfdir}/terminal-colors.d/nmcli.disable
}

# NetworkManager needs to be started after DBUS
INITSCRIPT_NAME = "networkmanager"
INITSCRIPT_PARAMS = "start 03 2 3 4 5 . stop 80 0 6 1 ."
