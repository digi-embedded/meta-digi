# Copyright (C) 2022-2024 Digi International.

require recipes-digi/dey-examples/connectcore-demo-example.inc

WESTON_SERVICE ?= "weston.service"
WESTON_SERVICE:ccmp15 ?= "weston-launch.service"

CC_DEMO_DISPLAY ?= "wayland-0"
CC_DEMO_DISPLAY:ccmp15 ?= "wayland-1"
CC_DEMO_DISPLAY:ccimx93 ?= "wayland-1"
CC_DEMO_ENV ?= "DISPLAY=:0.0 XDG_RUNTIME_DIR=/run/user/0 WAYLAND_DISPLAY=\$\{DEMO_DISPLAY\}"
CC_DEMO_ENV:ccimx6ul ?= ""

FILESEXTRAPATHS:prepend := "${THISDIR}/../../../../recipes-digi/dey-examples/connectcore-demo-example:"

SRC_URI += " \
    file://connectcore-demo-example-init \
    file://connectcore-demo-example.service \
"

do_install:append() {
	# Install systemd service
	if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
		# Install systemd unit files
		install -d ${D}${systemd_unitdir}/system
		install -m 0644 ${WORKDIR}/connectcore-demo-example.service ${D}${systemd_unitdir}/system/
		sed -i -e "s,##WESTON_SERVICE##,${WESTON_SERVICE},g" \
		      "${D}${systemd_unitdir}/system/connectcore-demo-example.service"
	fi

	# Install connectcore-demo-example-init
	install -d ${D}${sysconfdir}/init.d/
	install -m 755 ${WORKDIR}/connectcore-demo-example-init ${D}${sysconfdir}/connectcore-demo-example
	sed -i -e "s@##CC_DEMO_ENV##@${CC_DEMO_ENV}@g" \
	       -e "s@##CC_DEMO_DISPLAY##@${CC_DEMO_DISPLAY}@g" \
	       "${D}${sysconfdir}/connectcore-demo-example"
	ln -sf ${sysconfdir}/connectcore-demo-example ${D}${sysconfdir}/init.d/connectcore-demo-example
}

FILES:${PN}:append = " \
    ${systemd_unitdir}/system/connectcore-demo-example.service \
    ${sysconfdir}/connectcore-demo-example \
    ${sysconfdir}/init.d/connectcore-demo-example \
"

RDEPENDS:${PN} += "cog"

# 'connectcore-demo-example-init' script uses '/etc/init.d/functions'
RDEPENDS:${PN} += "initscripts-functions"

INITSCRIPT_PACKAGES += "${PN}"
INITSCRIPT_NAME:${PN} = "connectcore-demo-example"
INITSCRIPT_PARAMS:${PN} = "start 19 2 3 4 5 . stop 21 0 1 6 ."

SYSTEMD_PACKAGES += "${PN}"
SYSTEMD_SERVICE:${PN} = "connectcore-demo-example.service"

RREPLACES:${PN} = "connectcore-demo-example"
RCONFLICTS:${PN} = "connectcore-demo-example"
