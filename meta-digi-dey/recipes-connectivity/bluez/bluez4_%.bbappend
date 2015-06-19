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

SRC_URI += "file://audio.conf"

do_install_append() {
    install -m 0644 ${WORKDIR}/audio.conf ${D}/${sysconfdir}/bluetooth/
}
