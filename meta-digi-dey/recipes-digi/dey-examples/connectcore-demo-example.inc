# Copyright (C) 2022 Digi International.

require dey-examples-src.inc

SUMMARY = "Connectcore demo"
DESCRIPTION = "Connectcore demo"
SECTION = "examples"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI += " \
    file://connectcore-demo-example-init \
    file://connectcore-demo-example.service \
"

RDEPENDS:${PN} = " \
    busybox-httpd \
    libdigiapix-python3 \
    mpg123 \
    python3-connectcore-ble \
    python3-core \
    python3-httplib2 \
    python3-json \
    python3-logging \
    python3-netserver \
"

S = "${WORKDIR}/connectcore-demo-example"

inherit systemd update-rc.d

do_install() {
    install -d ${D}/srv/www
    cp -r ${WORKDIR}/git/connectcore-demo-example/* ${D}/srv/www/

    # Remove unused images
    find ${D}/srv/www/static/images/ -type f -name '*_board.png' -not -name '${MACHINE}_board.png' -delete

    if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
        # Install systemd unit files
        install -d ${D}${systemd_unitdir}/system
        install -m 0644 ${WORKDIR}/connectcore-demo-example.service ${D}${systemd_unitdir}/system/
    fi

    # connectcore-demo-example-init
    install -d ${D}${sysconfdir}/init.d/
    install -m 755 ${WORKDIR}/connectcore-demo-example-init ${D}${sysconfdir}/connectcore-demo-example
    ln -sf /etc/connectcore-demo-example ${D}${sysconfdir}/init.d/connectcore-demo-example
}

FILES:${PN} += " \
    /srv/www/* \
    ${systemd_unitdir}/system/connectcore-demo-example.service \
    ${sysconfdir}/connectcore-demo-example \
    ${sysconfdir}/init.d/connectcore-demo-example \
"

INITSCRIPT_NAME = "connectcore-demo-example"
INITSCRIPT_PARAMS = "start 19 2 3 4 5 . stop 21 0 1 6 ."

SYSTEMD_SERVICE:${PN} = "connectcore-demo-example.service"

PACKAGE_ARCH = "${MACHINE_ARCH}"
