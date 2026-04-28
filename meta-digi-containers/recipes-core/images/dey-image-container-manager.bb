#
# Copyright (C) 2026, Digi International Inc.
#
DESCRIPTION = "Minimal DEY container manager image"

LICENSE = "MIT"

IMAGE_LINGUAS = ""

inherit image
inherit dey-image
inherit features_check

REQUIRED_DISTRO_FEATURES = "virtualization"

# Keep machine recommendations in sync with the BSP.
CONTAINER_MANAGER_MACHINE_EXTRA_RRECOMMENDS_EXCLUDE = " \
	${MACHINE_FIRMWARE} \
	${UDEV_EXTRA_RULES} \
"

# Remove kernel-image-* from the list, as adding them
# explicitly breaks STM rootfs generation.
CONTAINER_MANAGER_MACHINE_EXTRA_RRECOMMENDS = "${@' '.join( \
	dict.fromkeys( \
		pkg for pkg in d.getVar('MACHINE_EXTRA_RRECOMMENDS').split() \
		if not pkg.startswith('kernel-image-') \
		and pkg not in d.getVar( \
			'CONTAINER_MANAGER_MACHINE_EXTRA_RRECOMMENDS_EXCLUDE').split()))}"

# Device core support
CORE_ESSENTIALS = " \
	base-files \
	base-passwd \
	bluez5-init \
	busybox \
	busybox-syslog \
	ca-certificates \
	cryptodev-module \
	firmwared \
	libubootenv-bin \
	udev \
	${MACHINE_ESSENTIAL_EXTRA_RDEPENDS} \
	${MACHINE_EXTRA_RDEPENDS} \
	${CONTAINER_MANAGER_MACHINE_EXTRA_RRECOMMENDS} \
	${MACHINE_FIRMWARE} \
	${UDEV_EXTRA_RULES} \
	${VIRTUAL-RUNTIME_init_manager} \
"

# Tools and libraries
CORE_TOOLS = " \
	bluez5 \
	cccs-daemon \
	curl \
	dropbear \
	dualboot \
	ethtool \
	init-ifupdown \
	iproute2 \
	iw \
	libdigiapix \
	libdigiapix-python3 \
	libgpiod \
	libgpiod-tools \
	ncurses-terminfo-base \
	netbase \
	networkmanager \
	networkmanager-nmcli \
	networkmanager-wifi \
	os-release \
	openssl \
	packagegroup-dey-audio \
	sysinfo \
	vsftpd \
"

# Tools to manage containers
CONTAINER_MANAGEMENT = " \
	cc-container-mng \
	lxc-trimmed \
	podman-trimmed \
"

# Keep the systemd/udev hotplug support packages that are otherwise
# dropped when recommendations are disabled.
HOTPLUG_SUPPORT = " \
	systemd-conf \
	systemd-extra-utils \
	udev-hwdb \
"

NO_RECOMMENDATIONS = "1"

IMAGE_INSTALL = " \
	${CORE_ESSENTIALS} \
	${CORE_TOOLS} \
	${CONTAINER_MANAGEMENT} \
	${HOTPLUG_SUPPORT} \
"

# GPU firmware for ccimx95, it needs to be located in the image
IMAGE_INSTALL:append:ccimx95-dvk = " mali-imx"
