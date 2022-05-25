# Copyright (C) 2019-2021 Digi International.

FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += " \
    file://digi_background.png \
    file://profile \
"

# Remove duplicate entries for the ccimx6 platform
# and uncomment with an individual append
INI_UNCOMMENT_ASSIGNMENTS:remove:ccimx6 = "use-g2d=1"

INI_UNCOMMENT_ASSIGNMENTS:append:mx8ulp-nxp-bsp = " \
    use-g2d=1 \
"

WATCHDOG_SEC = "40"
WATCHDOG_SEC:mx8ulp-nxp-bsp = "240"

update_file() {
    if ! grep -q "$1" $3; then
        bbfatal $1 not found in $3
    fi
    sed -i -e "s,$1,$2," $3
}

do_install:append:ccimx6() {
    uncomment "use-g2d=1" ${D}${sysconfdir}/xdg/weston/weston.ini
}

do_install:append() {
    install -Dm0755 ${WORKDIR}/profile ${D}${sysconfdir}/profile.d/weston.sh

    # Add weston.log back, used by NXP for testing
    update_file "ExecStart=/usr/bin/weston " "ExecStart=/usr/bin/weston --log=\$\{XDG_RUNTIME_DIR\}/weston.log " ${D}${systemd_system_unitdir}/weston.service

    # FIXME: weston should be run as weston, not as root
    update_file "User=weston" "User=root" ${D}${systemd_system_unitdir}/weston.service
    update_file "Group=weston" "Group=root" ${D}${systemd_system_unitdir}/weston.service

    update_file "WatchdogSec=20" "WatchdogSec=${WATCHDOG_SEC}" ${D}${systemd_system_unitdir}/weston.service

    update_file "Before=graphical.target" "Before=multi-user.target" ${D}${systemd_system_unitdir}/weston.service
    update_file "WantedBy=graphical.target" "WantedBy=multi-user.target" ${D}${systemd_system_unitdir}/weston.service

    # Add custom background image
    install -d ${D}${datadir}/weston
    install ${WORKDIR}/digi_background.png ${D}${datadir}/weston
}

FILES:${PN} += "${datadir}/weston/digi_background.png"
