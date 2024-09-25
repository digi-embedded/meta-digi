#
# Copyright (C) 2012, Digi International Inc.
#
SUMMARY = "Network applications packagegroup for DEY image"

PACKAGE_ARCH = "${MACHINE_ARCH}"
inherit packagegroup

# Distro can override the following VIRTUAL-RUNTIME providers:
VIRTUAL-RUNTIME_ftp-server ?= "vsftpd"

VIRTUAL-RUNTIME_http-server ?= "busybox-httpd"
#VIRTUAL-RUNTIME_http-server ?= "cherokee"

# Choose between ethtool or mii-tool
VIRTUAL-RUNTIME_network-utils ?= "ethtool"
#VIRTUAL-RUNTIME_network-utils ?= "net-tools"

VIRTUAL-RUNTIME_snmp-manager ?= ""
#VIRTUAL-RUNTIME_snmp-manager ?= "net-snmp-server"

VIRTUAL-RUNTIME_ntp-client ?= "busybox-ntpd"

CELLULAR_PKGS = "\
    modemmanager \
    ppp \
"

RDEPENDS:${PN} = "\
	iproute2 \
	batctl \
	${@bb.utils.contains('DISTRO_FEATURES', 'cellular', '${CELLULAR_PKGS}', '', d)} \
	${VIRTUAL-RUNTIME_ftp-server} \
	${VIRTUAL-RUNTIME_http-server} \
	${VIRTUAL-RUNTIME_network-utils} \
	${VIRTUAL-RUNTIME_snmp-manager} \
	${@bb.utils.contains('DISTRO_FEATURES', 'systemd', '', '${VIRTUAL-RUNTIME_ntp-client}', d)} \
	${@bb.utils.contains('DISTRO_FEATURES', 'tsn', 'iproute2-tc', '', d)} \
	${@bb.utils.contains('DISTRO_FEATURES', 'tsn', 'linuxptp', '', d)} \
"
