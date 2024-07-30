# Copyright (C) 2013-2024, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:append:ccimx6 = " \
    file://asound.inline_play.state \
    file://asound.inline.state \
    file://asound.micro_play.state \
    file://asound.micro.state \
    file://asound.play.state \
"

do_install:append:ccimx6() {
	ln -sf asound.micro_play.state ${D}${localstatedir}/lib/alsa/asound.state
}
