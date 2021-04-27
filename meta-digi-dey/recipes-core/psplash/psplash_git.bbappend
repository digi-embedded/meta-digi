# Copyright (C) 2016 Digi International

FILESEXTRAPATHS_prepend_dey := "${THISDIR}/files:"

SRC_URI += " \
    file://0001-colors-modify-psplash-colors-to-match-Digi-scheme.patch \
    file://psplash-digi-bar.png \
"
do_patch_png () {
	cp ${WORKDIR}/psplash-digi-bar.png ${S}/base-images/psplash-bar.png
}
addtask patch_png after do_patch before do_configure
