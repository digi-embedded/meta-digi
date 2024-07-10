# Copyright (C) 2022,2023, Digi International Inc.

require dey-examples-src.inc

SUMMARY = "Connectcore demo"
DESCRIPTION = "Connectcore demo"
SECTION = "examples"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI += " \
    file://connectcore-demo-server-init \
    file://connectcore-demo-server.service \
    file://suspendtarget-connectcore-demo-server.service \
"

RDEPENDS:${PN}-server = " \
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

BOARD_IMAGE_FILE ?= "${MACHINE}_board.png"

# The ccimx6qpsbc re-uses the ccimx6sbc board image
BOARD_IMAGE_FILE:ccimx6qpsbc = "ccimx6sbc_board.png"

do_install() {
	install -d ${D}/srv/www
	cp -r ${WORKDIR}/git/connectcore-demo-example/* ${D}/srv/www/

	# Remove unused images
	find ${D}/srv/www/static/images/ -type f -name '*_board.png' -not -name '${BOARD_IMAGE_FILE}' -delete

	# Install systemd service
	if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
		# Install systemd unit files
		install -d ${D}${systemd_unitdir}/system
		install -m 0644 ${WORKDIR}/connectcore-demo-server.service ${D}${systemd_unitdir}/system/
		install -m 0644 ${WORKDIR}/suspendtarget-connectcore-demo-server.service ${D}${systemd_unitdir}/system/
	fi

	# Install connectcore-demo-server-init
	install -d ${D}${sysconfdir}/init.d/
	install -m 755 ${WORKDIR}/connectcore-demo-server-init ${D}${sysconfdir}/connectcore-demo-server
	ln -sf ${sysconfdir}/connectcore-demo-server ${D}${sysconfdir}/init.d/connectcore-demo-server
}

PACKAGES =+ "${PN}-server ${PN}-multimedia"

FILES:${PN} += " \
    /srv/www/* \
"

FILES:${PN}-multimedia += " \
    /srv/www/multimedia* \
    /srv/www/static/images/aquarium.jpg \
    /srv/www/static/images/big_buck_bunny.jpg \
    /srv/www/static/images/blob.jpg \
    /srv/www/static/images/cpu.png \
    /srv/www/static/images/electricflower.jpg \
    /srv/www/static/images/multiple-views.jpg \
    /srv/www/static/images/ram.png \
    /srv/www/static/images/spacerocks.jpg \
    /srv/www/static/images/toon-shading.jpg \
    /srv/www/static/js/multimedia.js \
"

FILES:${PN}-server = " \
    /srv/www/demoserver.py \
    ${systemd_unitdir}/system/connectcore-demo-server.service \
    ${systemd_unitdir}/system/suspendtarget-connectcore-demo-server.service \
    ${sysconfdir}/connectcore-demo-server \
    ${sysconfdir}/init.d/connectcore-demo-server \
"

RDEPENDS:${PN} = "${PN}-server"
RDEPENDS:${PN}-multimedia = " \
    ${PN} \
    video-examples \
    webglsamples \
"

INITSCRIPT_PACKAGES = "${PN}-server"
INITSCRIPT_NAME:${PN}-server = "connectcore-demo-server"
INITSCRIPT_PARAMS:${PN}-server = "start 20 2 3 4 5 . stop 21 0 1 6 ."

SYSTEMD_PACKAGES = "${PN}-server"
SYSTEMD_SERVICE:${PN}-server = " \
    connectcore-demo-server.service \
    suspendtarget-connectcore-demo-server.service \
"

PACKAGE_ARCH = "${MACHINE_ARCH}"
