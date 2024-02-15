# Copyright (C) 2016 Digi International

FILESEXTRAPATHS:prepend:dey := "${THISDIR}/files:"

SRC_URI:append:dey = " \
    file://0001-colors-modify-psplash-colors-to-match-Digi-scheme.patch \
    file://psplash-digi-bar.png \
"
do_patch_png () {
}
do_patch_png:dey () {
	cp ${WORKDIR}/psplash-digi-bar.png ${S}/base-images/psplash-bar.png
}
addtask patch_png after do_patch before do_configure
