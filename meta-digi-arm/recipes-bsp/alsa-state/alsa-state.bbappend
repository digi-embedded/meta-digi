# Copyright (C) 2013-2023 Digi International.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:append:ccimx6 = " \
    file://asound.inline_play.state \
    file://asound.inline.state \
    file://asound.micro_play.state \
    file://asound.micro.state \
    file://asound.play.state \
"

SRC_URI:append:ccimx6ul = " file://asound.state"
SRC_URI:append:ccimx8x = " file://asound.state"
SRC_URI:append:ccimx8m = " file://asound.state"
SRC_URI:append:ccimx93 = " file://asound.state"
SRC_URI:append:ccmp1 = " file://asound.state"

do_install:append:ccimx6() {
	ln -sf asound.micro_play.state ${D}${localstatedir}/lib/alsa/asound.state
}
