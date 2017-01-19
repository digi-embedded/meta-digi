# Copyright (C) 2017 Digi International

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI_append_ccimx6ulsbc = " file://0001-pulseaudio-keep-headphones-volume-in-platforms-witho.patch"
SRC_URI_append_ccimx6sbc = " \
    file://hdmi_hotplug.sh \
    file://dey-audio-hdmi.conf \
    file://dey-audio-sgtl5000.conf \
"

do_install_append_ccimx6sbc() {
	install -d ${D}${sysconfdir}/udev/scripts
	install -m 0755 ${WORKDIR}/hdmi_hotplug.sh ${D}${sysconfdir}/udev/scripts

	install -d ${D}${datadir}/pulseaudio/alsa-mixer/profile-sets
	install -m 0644 ${WORKDIR}/dey-audio-hdmi.conf ${D}${datadir}/pulseaudio/alsa-mixer/profile-sets
	install -m 0644 ${WORKDIR}/dey-audio-sgtl5000.conf ${D}${datadir}/pulseaudio/alsa-mixer/profile-sets

	sed -i -e '/load-module module-suspend-on-idle/{s,$, timeout=0,g}' ${D}${sysconfdir}/pulse/default.pa

	cat >> ${D}${base_libdir}/udev/rules.d/90-pulseaudio.rules <<-_EOL_

		# Digi ConnectCore 6 SBC HDMI
		ATTRS{id}=="imxhdmisoc", ENV{PULSE_PROFILE_SET}="dey-audio-hdmi.conf"
		# Digi ConnectCore 6 SBC SGTL5000
		ATTRS{id}=="sgtl5000audio", ENV{PULSE_PROFILE_SET}="dey-audio-sgtl5000.conf"

		SUBSYSTEM=="platform", KERNEL=="*hdmi_video", ACTION=="change", RUN+="/etc/udev/scripts/hdmi_hotplug.sh"
	_EOL_
}

PACKAGE_ARCH = "${MACHINE_ARCH}"

# The card-detect binary is only necessary for the HDMI hotplug to work on the ccimx6sbc
RDEPENDS_${PN}_append_ccimx6sbc = " card-detect"
