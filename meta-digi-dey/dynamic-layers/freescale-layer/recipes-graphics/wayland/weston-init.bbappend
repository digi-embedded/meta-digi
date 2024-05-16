# Copyright (C) 2019-2024, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += " \
    file://digi_background.png \
    file://profile \
"

update_file() {
    if ! grep -q "$1" $3; then
        bbfatal $1 not found in $3
    fi
    sed -i -e "s,$1,$2," $3
}

do_install:append() {
    if ${@bb.utils.contains('DISTRO_FEATURES','systemd','true','false',d)}; then
        # Add weston.log back, used by NXP for testing
        update_file "ExecStart=/usr/bin/weston " "ExecStart=/usr/bin/weston --log=\$\{XDG_RUNTIME_DIR\}/weston.log " ${D}${systemd_system_unitdir}/weston.service

        # FIXME: weston should be run as weston, not as root
        update_file "User=weston" "User=root" ${D}${systemd_system_unitdir}/weston.service
        update_file "Group=weston" "Group=root" ${D}${systemd_system_unitdir}/weston.service
    else
        # Install weston-socket.sh for sysvinit as well
        install -D -p -m0644 ${WORKDIR}/weston-socket.sh ${D}${sysconfdir}/profile.d/weston-socket.sh
    fi
}

# DEY customizations
do_install:append() {
    install -Dm0755 ${WORKDIR}/profile ${D}${sysconfdir}/profile.d/weston.sh
    install -Dm0644 ${WORKDIR}/digi_background.png ${D}${datadir}/weston/digi_background.png

    printf "\n[launcher]\nicon=${datadir}/weston/terminal.png\npath=${bindir}/weston-terminal\n" >> ${D}${sysconfdir}/xdg/weston/weston.ini
}

do_install:append:ccimx93() {
    install -d ${D}${sysconfdir}/default/
    echo "QMLSCENE_DEVICE=softwarecontext" >> ${D}${sysconfdir}/default/weston
}

FILES:${PN} += "${datadir}/weston/digi_background.png"
