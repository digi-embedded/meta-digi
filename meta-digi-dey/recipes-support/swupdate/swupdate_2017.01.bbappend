# Copyright (C) 2016-2018 Digi International Inc.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI += "\
     file://swupdate-usb.rules \
     file://swupdate-usb@.service \
     file://swupdate-progress.service \
"

do_configure_append() {
	# If Trustfence is enabled, enable the signing support in the
	# '.config' file.
	if [ "${TRUSTFENCE_SIGN}" = "1" ]; then
		echo "CONFIG_SIGNED_IMAGES=y" >> ${S}/.config
		cml1_do_configure
	fi
}

do_compile_append() {
	unset LDFLAGS
	oe_runmake progress_unstripped
	cp progress_unstripped progress
}

do_install_append() {
	# Copy the 'progress' binary.
	install -d ${D}${bindir}/
	install -m 0755 progress ${D}${bindir}/
	# Rename 'swupdate' binary
	mv ${D}${bindir}/swupdate_unstripped ${D}${bindir}/swupdate
}
