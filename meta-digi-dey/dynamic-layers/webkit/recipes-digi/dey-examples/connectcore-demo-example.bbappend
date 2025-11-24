# Copyright 2025, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/connectcore-demo-example:"

SRC_URI += "\
    file://connectcore-demo-example-webkit-init \
    file://connectcore-demo-example-webkit.service \
"

CC_DEMO_ENV ?= "DISPLAY=:0.0 XDG_RUNTIME_DIR=/run/user/0 WAYLAND_DISPLAY=\$\{DEMO_DISPLAY\}"
CC_DEMO_ENV:ccimx6ul ?= ""

WESTON_SERVICE ?= "weston.service"
WESTON_SERVICE:ccmp15 ?= "weston-launch.service"
WESTON_SERVICE:ccmp25 ?= "weston-launch.service"

do_install:append() {
    # Install the webkit systemd unit
    if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
        install -d ${D}${systemd_unitdir}/system
        install -m 0644 ${WORKDIR}/connectcore-demo-example-webkit.service ${D}${systemd_unitdir}/system/
        sed -i -e "s,##WESTON_SERVICE##,${WESTON_SERVICE},g" \
            ${D}${systemd_unitdir}/system/connectcore-demo-example-webkit.service
    fi

    # Install the init script
    install -d ${D}${sysconfdir}/init.d/
    install -m 755 ${WORKDIR}/connectcore-demo-example-webkit-init ${D}${sysconfdir}/connectcore-demo-example-webkit
    sed -i  -e "s@##CC_DEMO_ENV##@${CC_DEMO_ENV}@g" \
            -e "s@##CC_DEMO_DISPLAY##@${WAYLAND_DISPLAY}@g" \
            ${D}${sysconfdir}/connectcore-demo-example-webkit
    ln -sf ../connectcore-demo-example-webkit ${D}${sysconfdir}/init.d/
}

PACKAGES =+ "${PN}-webkit"

FILES:${PN}-webkit += "\
    ${systemd_unitdir}/system/connectcore-demo-example-webkit.service \
    ${sysconfdir}/connectcore-demo-example-webkit \
    ${sysconfdir}/init.d/connectcore-demo-example-webkit \
"

RDEPENDS:${PN}-webkit = "initscripts-functions"

INITSCRIPT_PACKAGES += "${PN}-webkit"
INITSCRIPT_NAME:${PN}-webkit = "connectcore-demo-example-webkit"
INITSCRIPT_PARAMS:${PN}-webkit = "start 19 2 3 4 5 . stop 21 0 1 6 ."

SYSTEMD_PACKAGES += "${PN}-webkit"
SYSTEMD_SERVICE:${PN}-webkit = "connectcore-demo-example-webkit.service"
