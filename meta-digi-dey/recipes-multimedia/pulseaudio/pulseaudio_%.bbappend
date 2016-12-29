# Copyright (C) 2017 Digi International

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI_append_ccimx6ulsbc = " file://0001-pulseaudio-keep-headphones-volume-in-platforms-witho.patch"
SRC_URI_append_ccimx6sbc = " file://hdmi_hotplug.sh"

do_install_append_ccimx6sbc() {
	install -d ${D}${sysconfdir}/udev/scripts
	install -m 0755 ${WORKDIR}/hdmi_hotplug.sh ${D}${sysconfdir}/udev/scripts

	cat >> ${D}${base_libdir}/udev/rules.d/90-pulseaudio.rules <<-_EOL_

		SUBSYSTEM=="platform", KERNEL=="*hdmi_video", ACTION=="change", RUN+="/etc/udev/scripts/hdmi_hotplug.sh"
	_EOL_
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
