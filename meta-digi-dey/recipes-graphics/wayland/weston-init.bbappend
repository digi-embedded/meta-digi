# Copyright (C) 2019 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += " \
    file://digi_background.png \
    file://profile \
"


# To customize weston.ini, start by setting the desired assignment in weston.ini,
# commented out. For example:
#     #xwayland=true
# Then add the assignment to INI_UNCOMMENT_ASSIGNMENTS.
INI_UNCOMMENT_ASSIGNMENTS_append_mx8mp = " \
    use-g2d=1 \
"
INI_UNCOMMENT_ASSIGNMENTS_append_mx8mq = " \
    drm-device=card0 \
"
INI_UNCOMMENT_ASSIGNMENTS_append_mx8 = " \
    repaint-window=16 \
"

# Digi: use g2d on ccimx6sbc to fix desktop window issue
# Also needed to workaround an HDMI hotplug issue on the ccimx6qpsbc
INI_UNCOMMENT_ASSIGNMENTS_append_ccimx6 = " \
    use-g2d=1 \
"

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
