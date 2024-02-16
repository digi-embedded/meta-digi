# Copyright (C) 2022-2024 Digi International Inc.

SUMMARY = "Crank Demo"
HOMEPAGE = "https://www.cranksoftware.com/"
LICENSE = "CLOSED"

DEPENDS = "crank-sbengine"

SRC_URI = " \
    http:///not/exist/crank-demos-${PV}.tar.gz \
    file://crank-demo.service \
    file://crank-demo-init \
"
SRC_URI[sha256sum] = "90a7fc258cdaa5f9afcf57224da8bbc5a330b957db88335f555369123a1432ab"

WESTON_SERVICE ?= "weston.service"
WESTON_SERVICE:ccmp15 ?= "weston-launch.service"

CRANK_DEMOS_TARBALL_PATH ?= ""
CRANK_DEMO_DISPLAY ?= "wayland-0"
CRANK_DEMO_DISPLAY:ccmp15 ?= "wayland-1"
CRANK_DEMO_DISPLAY:ccimx93 ?= "wayland-1"
CRANK_DEMO_ENV ?= "DISPLAY=:0.0 XDG_RUNTIME_DIR=/run/user/0 WAYLAND_DISPLAY=\$\{DEMO_DISPLAY\}"
CRANK_DEMO_ENV:ccimx6ul ?= ""
CRANK_DEMO_OPTIONS ?= "-orender_mgr,multisample=0"
CRANK_DEMO_OPTIONS:ccimx6ul ?= "-orender_mgr,multisample=0 -odev-input,mouse=/dev/input/mouse0 -oscreen_mgr,swcursor"
CRANK_DEMO_PATH ?= "${datadir}/crank/apps/OpenGL_WideScreen/1280x720.gapp"

# The tarball is only available for downloading after registration, so provide
# a PREMIRROR to a local directory that can be configured in the project's
# local.conf file using CRANK_DEMOS_TARBALL_PATH variable.
python() {
    crank_demos_tarball_path = d.getVar('CRANK_DEMOS_TARBALL_PATH')
    if crank_demos_tarball_path:
        premirrors = d.getVar('PREMIRRORS')
        d.setVar('PREMIRRORS', "http:///not/exist/crank-demos-.* %s \\n %s" % (crank_demos_tarball_path, premirrors))
    crank_demos_tarball_sha256 = d.getVar('CRANK_DEMOS_TARBALL_SHA256')
    if crank_demos_tarball_sha256:
        d.setVarFlag("SRC_URI", "sha256sum", crank_demos_tarball_sha256)
}

inherit systemd update-rc.d

# Disable tasks not needed for the binary package
do_configure[noexec] = "1"

do_compile () {
	for f in ${S}/*; do
		if [ -d "${f}/source_code" ]; then
			oe_runmake -C "${f}/source_code"
		fi
	done
}

do_install () {
	install -d -m 0755 ${D}${datadir}/crank/apps

	# Install Crank demos
	tar --no-same-owner --exclude='EULA.pdf' --exclude='*/source_code' -cpf - -C ${S} . \
		| tar --no-same-owner -xpf - -C ${D}${datadir}/crank/apps

	# Install required binaries
	for f in ${S}/*; do
		if [ -d "${f}/source_code" ]; then
			oe_runmake DESTDIR=${D}${datadir}/crank/apps/$(basename ${f}) -C "${f}/source_code" install
		fi
	done

	# Install systemd service
	if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
		# Install systemd unit files
		install -d ${D}${systemd_unitdir}/system
		install -m 0644 ${WORKDIR}/crank-demo.service ${D}${systemd_unitdir}/system/
		sed -i -e "s,##WESTON_SERVICE##,${WESTON_SERVICE},g" \
		      "${D}${systemd_unitdir}/system/crank-demo.service"
	fi

	# Install wrapper bootscript to launch Crank demo on boot
	install -d ${D}${sysconfdir}/init.d
	install -m 0755 ${WORKDIR}/crank-demo-init ${D}${sysconfdir}/crank-demo
	sed -i -e "s@##CRANK_DEMO_PATH##@${CRANK_DEMO_PATH}@g" \
	       -e "s@##CRANK_DEMO_OPTIONS##@${CRANK_DEMO_OPTIONS}@g" \
	       -e "s@##CRANK_DEMO_ENV##@${CRANK_DEMO_ENV}@g" \
	       -e "s@##CRANK_DEMO_DISPLAY##@${CRANK_DEMO_DISPLAY}@g" \
	       "${D}${sysconfdir}/crank-demo"
	ln -sf ${sysconfdir}/crank-demo ${D}${sysconfdir}/init.d/crank-demo
}

FILES:${PN} = " \
    ${datadir}/crank/apps/* \
    ${sysconfdir}/crank-demo \
    ${sysconfdir}/init.d/crank-demo \
    ${systemd_unitdir}/system/crank-demo.service \
"

INITSCRIPT_NAME = "crank-demo"
INITSCRIPT_PARAMS = "defaults 90 10"

SYSTEMD_SERVICE:${PN} = "crank-demo.service"

RDEPENDS:${PN} += "crank-sbengine"
