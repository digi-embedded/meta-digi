FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI += "file://hdmi_hotplug.sh"

# Do not include module-udev-detect so that module-detect is used instead
RDEPENDS_pulseaudio-server_remove = "pulseaudio-module-udev-detect"

do_install_append() {
	install -d ${D}${sysconfdir}/udev/scripts
	install -m 0755 ${WORKDIR}/hdmi_hotplug.sh ${D}${sysconfdir}/udev/scripts

	cat >> ${D}${base_libdir}/udev/rules.d/90-pulseaudio.rules <<-_EOL_

		SUBSYSTEM=="platform", KERNEL=="*hdmi_video", ACTION=="change", RUN+="/etc/udev/scripts/hdmi_hotplug.sh"
	_EOL_
}
