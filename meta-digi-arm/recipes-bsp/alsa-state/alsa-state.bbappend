# Copyright (C) 2013 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI_append_ccimx6 = " \
    file://asound.inline_play.state \
    file://asound.inline.state \
    file://asound.micro_play.state \
    file://asound.micro.state \
    file://asound.play.state \
"

SRC_URI_append_ccimx6ul = " file://asound.state"

do_install_append_ccimx6() {
	ln -sf asound.micro_play.state ${D}${localstatedir}/lib/alsa/asound.state
}
