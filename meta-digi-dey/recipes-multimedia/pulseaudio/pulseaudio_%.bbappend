# Copyright (C) 2019-2022 Digi International

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

# Configuration files for SGTL500 sound card
CFG_SGTL5000 = " \
    file://sgtl5000/dey-audio-sgtl5000.conf \
    file://sgtl5000/90-pulseaudio.rules \
"

# Configuration files for MAX98089 sound card
CFG_MAX98089 = " \
    file://max98089/dey-audio-max98089.conf \
    file://max98089/90-pulseaudio.rules \
"

# Configuration files for HDMI sound card
CFG_HDMI = " \
    file://hdmi/hdmi_hotplug.sh \
    file://hdmi/dey-audio-hdmi.conf \
    file://hdmi/90-pulseaudio-hdmi.rules \
"
SOUND_CARD ?= "max98089"
SOUND_CARD:ccimx6 ?= "sgtl5000"

AUDIO_HDMI ?= ""
AUDIO_HDMI:ccimx6 = "yes"

SRC_URI:append = " \
    file://0001-bluetooth-Only-remove-cards-belonging-to-the-device.patch \
    ${@oe.utils.conditional('SOUND_CARD', 'sgtl5000', '${CFG_SGTL5000}', '', d)} \
    ${@oe.utils.conditional('SOUND_CARD', 'max98089', '${CFG_MAX98089}', '', d)} \
    ${@oe.utils.conditional('AUDIO_HDMI', 'yes', '${CFG_HDMI}', '', d)} \
    file://pulseaudio-init \
    file://pulseaudio-system.service \
"

SRC_URI:append:ccimx6ulsbc = " \
    file://0001-pulseaudio-keep-headphones-volume-in-platforms-witho.patch \
"

SRC_URI:append:ccmp1 = " \
    file://daemon.conf \
    file://0001-pulseaudio-keep-headphones-volume-in-platforms-witho.patch \
"

# This default setting should be added on all i.MX SoC,
# For now, the setting for mx6(including mx6ul & mx6sll)/mx7 has been upstreamed
SRC_URI:append:mx8-nxp-bsp = " \
    file://daemon.conf \
    file://default.pa \
"

# Enable allow-autospawn-for-root as default
PACKAGECONFIG:append = " autospawn-for-root"

EXTRA_OECONF:append:ccimx6 = " --disable-memfd"

FILES:${PN}-server:append = " \
    ${systemd_unitdir}/* \
    ${sysconfdir}/pulseaudio-init \
    ${sysconfdir}/init.d/pulseaudio-init \
"

inherit update-rc.d

INITSCRIPT_PACKAGES += "${PN}-server"
INITSCRIPT_NAME = "pulseaudio-init"
INITSCRIPT_PARAMS = "start 19 2 3 4 5 . stop 21 0 1 6 ."

SYSTEMD_SERVICE:${PN}-server = "pulseaudio-system.service"
SYSTEMD_PACKAGES = "${PN}-server"

do_install:append() {
	install -d ${D}${datadir}/pulseaudio/alsa-mixer/profile-sets
	install -m 0644 ${WORKDIR}/${SOUND_CARD}/dey-audio-*.conf ${D}${datadir}/pulseaudio/alsa-mixer/profile-sets

	install -d ${D}${base_libdir}/udev/rules.d
	install -m 0644 ${WORKDIR}/${SOUND_CARD}/90-pulseaudio.rules ${D}${base_libdir}/udev/rules.d

	# INITSCRIPT
	install -d ${D}${sysconfdir}/init.d/
	install -m 0755 ${WORKDIR}/pulseaudio-init ${D}${sysconfdir}/pulseaudio-init
	ln -sf /etc/pulseaudio-init ${D}${sysconfdir}/init.d/pulseaudio-init
	# SYSTEMD
	install -d ${D}${systemd_unitdir}/system
	install -m 0644 ${WORKDIR}/pulseaudio-system.service ${D}/${systemd_unitdir}/system

	# Remove pid file entry for non-graphical backend
	if [ "${IS_HEADLESS}" = "true" ]; then
		sed -i -e "/PIDFile/d" ${D}/${systemd_unitdir}/system/pulseaudio-system.service
	fi

	# Configuration files for HDMI sound card
	if [ "${AUDIO_HDMI}" = "yes" ]; then
		install -d ${D}${sysconfdir}/udev/scripts
		install -m 0755 ${WORKDIR}/hdmi/hdmi_hotplug.sh ${D}${sysconfdir}/udev/scripts
		install -m 0644 ${WORKDIR}/hdmi/dey-audio-hdmi.conf ${D}${datadir}/pulseaudio/alsa-mixer/profile-sets
		install -m 0644 ${WORKDIR}/hdmi/90-pulseaudio-hdmi.rules ${D}${base_libdir}/udev/rules.d
	fi

	sed -i -e '/load-module module-suspend-on-idle/{s,$, timeout=0,g}' ${D}${sysconfdir}/pulse/default.pa
}

# Pulse audio configuration files installation
do_install:append:ccmp1() {
	if [ -e "${WORKDIR}/daemon.conf" ]; then
		install -m 0644 ${WORKDIR}/daemon.conf ${D}/${sysconfdir}/pulse/daemon.conf
	fi
}

PACKAGE_ARCH = "${MACHINE_ARCH}"

# The card-detect binary is only necessary for the HDMI hotplug to work on the ccimx6sbc/ccimx6qpsbc
RDEPENDS:${PN}:append:ccimx6 = " card-detect"
