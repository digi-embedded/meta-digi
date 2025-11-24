# Copyright (C) 2025, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/connectcore-demo-example:"

SRC_URI += "file://connectcore-demo-example-chromium.service"

do_install:append() {
    # Install the chromium systemd unit
    if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
        install -d ${D}${systemd_system_unitdir}
        install -m 0644 ${WORKDIR}/connectcore-demo-example-chromium.service ${D}${systemd_system_unitdir}/
    fi
}

PACKAGES =+ "${PN}-chromium"
FILES:${PN}-chromium += "${systemd_system_unitdir}/connectcore-demo-example-chromium.service"
RDEPENDS:${PN}-chromium = "${PN}-multimedia"

SYSTEMD_PACKAGES += "${PN}-chromium"
SYSTEMD_SERVICE:${PN}-chromium = "connectcore-demo-example-chromium.service"
