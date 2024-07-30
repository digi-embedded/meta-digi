# Copyright (C) 2019-2023, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += " \
    file://digi_background.png \
    file://profile \
"
SRC_URI:append:ccimx93 = " file://weston-socket.sh"

INI_UNCOMMENT_ASSIGNMENTS:append:mx9-nxp-bsp = " \
    repaint-window=16 \
"
INI_UNCOMMENT_ASSIGNMENTS:append:mx93-nxp-bsp = " \
    gbm-format=argb8888 \
    use-g2d=true \
"

update_file() {
    if ! grep -q "$1" $3; then
        bbfatal $1 not found in $3
    fi
    sed -i -e "s,$1,$2," $3
}

do_install:append() {
    # Add weston.log back, used by NXP for testing
    update_file "ExecStart=/usr/bin/weston " "ExecStart=/usr/bin/weston --log=\$\{XDG_RUNTIME_DIR\}/weston.log " ${D}${systemd_system_unitdir}/weston.service

    # FIXME: weston should be run as weston, not as root
    update_file "User=weston" "User=root" ${D}${systemd_system_unitdir}/weston.service
    update_file "Group=weston" "Group=root" ${D}${systemd_system_unitdir}/weston.service
}

# DEY customizations
do_install:append() {
    install -Dm0755 ${WORKDIR}/profile ${D}${sysconfdir}/profile.d/weston.sh
    install -Dm0644 ${WORKDIR}/digi_background.png ${D}${datadir}/weston/digi_background.png

    printf "\n[launcher]\nicon=${datadir}/weston/terminal.png\npath=${bindir}/weston-terminal\n" >> ${D}${sysconfdir}/xdg/weston/weston.ini
}

do_install:append:ccimx93() {
    # The ccimx93 uses a new version of weston where 'weston-socket.sh' supercedes 'weston.sh'
    \rm -f ${D}${sysconfdir}/profile.d/weston.sh
    install -Dm0644 ${WORKDIR}/weston-socket.sh ${D}${sysconfdir}/profile.d/weston-socket.sh

    install -d ${D}${sysconfdir}/default/
    echo "QMLSCENE_DEVICE=softwarecontext" >> ${D}${sysconfdir}/default/weston
}

FILES:${PN} += "${datadir}/weston/digi_background.png"
