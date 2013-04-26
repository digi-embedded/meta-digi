# Copyright (C) 2013 Digi International.

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}/${MACHINE}:"

SRC_URI_append_ccardimx28js = " \
	file://asound.inline_play.state \
	file://asound.inline.state \
	file://asound.micro_play.state \
	file://asound.micro.state \
	file://asound.play.state \
"

SRC_URI_append_mx5 = " \
	file://asound.inline_play.state \
	file://asound.inline.state \
	file://asound.micro_play.state \
	file://asound.micro.state \
	file://asound.play.state \
"
