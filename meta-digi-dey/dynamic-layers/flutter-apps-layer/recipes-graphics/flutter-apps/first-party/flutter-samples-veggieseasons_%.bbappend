# Copyright (C) 2025, Digi International Inc.

FILESEXTRAPATHS:prepend:dey := "${THISDIR}/files:"

SRC_URI:append = " \
    file://flutter-demo-init \
    file://flutter-demo-init.service \
"

FLUTTER_DEMO_NAME ?= "${PN}"
FLUTTER_MODE ?= "${FLUTTER_APP_RUNTIME_MODES}"

inherit update-rc.d systemd

do_install:append() {
	# Install systemd service
	if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
		# Install systemd unit files
		install -d ${D}${systemd_unitdir}/system
		install -m 0644 ${WORKDIR}/flutter-demo-init.service ${D}${systemd_unitdir}/system/
		sed -i -e "s,##FLUTTER_DEMO_NAME##,${FLUTTER_DEMO_NAME},g" \
			"${D}${systemd_unitdir}/system/flutter-demo-init.service"
	fi

	# Install wrapper bootscript to launch Flutter demo on boot
	install -d ${D}${sysconfdir}/init.d
	install -m 0755 ${WORKDIR}/flutter-demo-init ${D}${sysconfdir}/flutter-demo-init
	ln -sf ${sysconfdir}/flutter-demo-init ${D}${sysconfdir}/init.d/flutter-demo-init
	sed -i -e "s@##FLUTTER_SDK_TAG##@${FLUTTER_SDK_TAG}@g" \
		   -e "s@##FLUTTER_MODE##@${FLUTTER_MODE}@g" \
		   -e "s@##FLUTTER_DEMO_NAME##@${FLUTTER_DEMO_NAME}@g" \
		   "${D}${sysconfdir}/flutter-demo-init"
}

PACKAGES =+ "${PN}-init"
FILES:${PN}-init = " \
    ${sysconfdir}/flutter-demo-init \
    ${sysconfdir}/init.d/flutter-demo-init \
    ${systemd_unitdir}/system/flutter-demo-init.service \
"

INITSCRIPT_PACKAGES += "${PN}-init"
INITSCRIPT_NAME:${PN}-init = "flutter-demo-init"
INITSCRIPT_PARAMS:${PN}-init = "start 99 3 5 . stop 20 0 1 2 6 ."

SYSTEMD_PACKAGES = "${PN}-init"
SYSTEMD_SERVICE:${PN}-init = "flutter-demo-init.service"
