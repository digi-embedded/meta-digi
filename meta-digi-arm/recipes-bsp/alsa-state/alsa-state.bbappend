# Copyright (C) 2013 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}/${PREFERRED_VERSION_linux-dey}:"

SRC_URI += " \
    file://asound.inline_play.state \
    file://asound.inline.state \
    file://asound.micro_play.state \
    file://asound.micro.state \
    file://asound.play.state \
"

do_install_append() {
	ln -sf asound.micro_play.state ${D}${localstatedir}/lib/alsa/asound.state
}
