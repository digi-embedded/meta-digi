# Copyright (C) 2019 Digi International

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

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
SOUND_CARD_ccimx6 ?= "sgtl5000"

AUDIO_HDMI ?= ""
AUDIO_HDMI_ccimx6 = "yes"

SRC_URI_append = " \
    file://0001-Fix-pulseaudio-mutex-issue-when-do-pause-in-gstreame.patch \
    file://0002-Revert-bluetooth-Fix-crash-when-disabling-Bluetooth-.patch \
    ${@oe.utils.conditional('SOUND_CARD', 'sgtl5000', '${CFG_SGTL5000}', '', d)} \
    ${@oe.utils.conditional('SOUND_CARD', 'max98089', '${CFG_MAX98089}', '', d)} \
    ${@oe.utils.conditional('AUDIO_HDMI', 'yes', '${CFG_HDMI}', '', d)} \
    file://pulseaudio-init \
    file://pulseaudio-system.service \
"

SRC_URI_append_ccimx6ulsbc = " \
    file://0001-pulseaudio-keep-headphones-volume-in-platforms-witho.patch \
"

# This default setting should be added on all i.MX SoC,
# For now, the setting for mx6(including mx6ul & mx6sll)/mx7 has been upstreamed
SRC_URI_append_mx8 = " \
    file://daemon.conf \
    file://default.pa \
"

# Enable allow-autospawn-for-root as default
PACKAGECONFIG_append = " autospawn-for-root"

EXTRA_OECONF_append_ccimx6 = " --disable-memfd"

FILES_${PN}-server_append = " ${systemd_unitdir}/* ${sysconfdir}/pulseaudio-init"

SYSTEMD_SERVICE_${PN}-server = "pulseaudio-system.service"
SYSTEMD_PACKAGES = "${PN}-server"

do_install_append() {
	install -d ${D}${datadir}/pulseaudio/alsa-mixer/profile-sets
	install -m 0644 ${WORKDIR}/${SOUND_CARD}/dey-audio-*.conf ${D}${datadir}/pulseaudio/alsa-mixer/profile-sets

	install -d ${D}${base_libdir}/udev/rules.d
	install -m 0644 ${WORKDIR}/${SOUND_CARD}/90-pulseaudio.rules ${D}${base_libdir}/udev/rules.d

	install -d ${D}${sysconfdir}
	install -m 0755 ${WORKDIR}/pulseaudio-init ${D}/${sysconfdir}

	install -d ${D}${systemd_unitdir}/system
	install -m 0644 ${WORKDIR}/pulseaudio-system.service ${D}/${systemd_unitdir}/system

	# Configuration files for HDMI sound card
	if [ "${AUDIO_HDMI}" = "yes" ]; then
		install -d ${D}${sysconfdir}/udev/scripts
		install -m 0755 ${WORKDIR}/hdmi/hdmi_hotplug.sh ${D}${sysconfdir}/udev/scripts
		install -m 0644 ${WORKDIR}/hdmi/dey-audio-hdmi.conf ${D}${datadir}/pulseaudio/alsa-mixer/profile-sets
		install -m 0644 ${WORKDIR}/hdmi/90-pulseaudio-hdmi.rules ${D}${base_libdir}/udev/rules.d
	fi

	sed -i -e '/load-module module-suspend-on-idle/{s,$, timeout=0,g}' ${D}${sysconfdir}/pulse/default.pa
}

PACKAGE_ARCH = "${MACHINE_ARCH}"

# The card-detect binary is only necessary for the HDMI hotplug to work on the ccimx6sbc/ccimx6qpsbc
RDEPENDS_${PN}_append_ccimx6 = " card-detect"
