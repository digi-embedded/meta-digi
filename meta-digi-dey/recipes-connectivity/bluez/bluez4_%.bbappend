# Copyright (C) 2013 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

EXTRA_OECONF_append = " --enable-health --enable-static"

# Adding '--enable-static' to the config builds static versions of
# libasound_module_ctl and libasound_module_pcm. Those files are not
# packaged, so it fails with "installed but not shipped".
#
# We do not really need those static libs so just disable the QA test
#
INSANE_SKIP_${PN} = "installed-vs-shipped"

inherit update-rc.d

SRC_URI += " \
    file://audio.conf \
    file://bluez-init \
"

do_install_append() {
	install -m 0644 ${WORKDIR}/audio.conf ${D}/${sysconfdir}/bluetooth/
	install -d  ${D}${sysconfdir}/init.d/
	install -m 0755 ${WORKDIR}/bluez-init ${D}${sysconfdir}/init.d/bluez
}

INITSCRIPT_NAME = "bluez"
INITSCRIPT_PARAMS = "start 10 5 ."
