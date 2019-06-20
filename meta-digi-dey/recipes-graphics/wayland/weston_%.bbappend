# Copyright (C) 2019 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += " \
    file://digi_background.png \
"

do_install_append() {
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
