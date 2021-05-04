# Copyright (C) 2019 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += " \
    file://digi_background.png \
    file://profile \
"

# Remove duplicate entries for the ccimx6 platform
# and uncomment with an individual append
INI_UNCOMMENT_ASSIGNMENTS_remove_ccimx6 = "use-g2d=1"

do_install_append_ccimx6() {
    uncomment "use-g2d=1" ${D}${sysconfdir}/xdg/weston/weston.ini
}

do_install_append() {
    install -Dm0755 ${WORKDIR}/profile ${D}${sysconfdir}/profile.d/weston.sh

    # Add custom background image
    install -d ${D}${datadir}/weston
    install ${WORKDIR}/digi_background.png ${D}${datadir}/weston

    # Customize weston ini file
    cat <<EOF >>${D}${sysconfdir}/xdg/weston/weston.ini

[shell]
background-image=/usr/share/weston/digi_background.png
background-type=scale-crop
EOF
}

FILES_${PN} += "${datadir}/weston/digi_background.png"
