#
# Copyright (C) 2012 Digi International.
#
DESCRIPTION = "Network applications task for DEL image"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/LICENSE;md5=3f40d7994397109285ec7b81fdeb3b58"
PACKAGE_ARCH = "${MACHINE_ARCH}"
ALLOW_EMPTY = "1"
PR = "r0"

#
# Set by the machine configuration with packages essential for device bootup
#
MACHINE_ESSENTIAL_EXTRA_RDEPENDS ?= ""
MACHINE_ESSENTIAL_EXTRA_RRECOMMENDS ?= ""

# Distro can override the following VIRTUAL-RUNTIME providers:
VIRTUAL-RUNTIME_ftp-server ?= ""
#VIRTUAL-RUNTIME_ftp-server ?= "vsftpd"

VIRTUAL-RUNTIME_http-server ?= ""
#VIRTUAL-RUNTIME_http-server ?= "cherokee"

# Choose between ethtool or mii-tool
VIRTUAL-RUNTIME_network-utils ?= "ethtool"
#VIRTUAL-RUNTIME_network-utils ?= "net-tools"

VIRTUAL-RUNTIME_snmp-manager ?= ""
#VIRTUAL-RUNTIME_snmp-manager ?= "net-snmp-server"

PACKAGES = "\
	task-del-network \
	task-del-network-dbg \
	task-del-network-dev \
    "

RDEPENDS_task-del-network = "\
	bridge-utils \
	ppp \
	iproute2 \
	${VIRTUAL-RUNTIME_ftp-server} \
	${VIRTUAL-RUNTIME_http-server} \
	${VIRTUAL-RUNTIME_network-utils} \
	${VIRTUAL-RUNTIME_snmp-manager} \
	${MACHINE_ESSENTIAL_EXTRA_RDEPENDS}"

RRECOMMENDS_task-del-network = "\
    ${MACHINE_ESSENTIAL_EXTRA_RRECOMMENDS}"


