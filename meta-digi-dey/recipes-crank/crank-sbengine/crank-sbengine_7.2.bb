# Copyright (C) 2022, Digi International Inc.

SUMMARY = "Crank Storyboard Engine"
HOMEPAGE = "https://www.cranksoftware.com/"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://EULA.pdf;md5=fcb6aca5219f44fea1c073405a378250"

SBENGINE_NAME:ccimx6ul = "linux-imx6yocto-armle-swrender-obj"
SBENGINE_NAME:ccimx6 = "linux-imx6yocto-armle-opengles_2.0-obj"
SBENGINE_NAME:ccimx8m = "linux-imx8yocto-armle-opengles_2.0-wayland-obj"
SBENGINE_NAME:ccimx8x = "linux-imx8yocto-armle-opengles_2.0-wayland-obj"
SBENGINE_NAME:ccmp15 = "linux-stmA5-armle-opengles_2.0-wayland-obj"

SRC_URI = " \
    http:///not/exist/crank-sbengine-${PV}.tar.gz \
    file://sb-launcher \
"
SRC_URI[md5sum] = "6faacbb61d4bd9e565ef71c91f20c6ec"
SRC_URI[sha256sum] = "79c9162c401dd6282321361d51f15ccef1608da7cde9030c2b72b9573e826056"

CRANK_ENGINE_TARBALL_PATH ?= ""

# The tarball is only available for downloading after registration, so provide
# a PREMIRROR to a local directory that can be configured in the project's
# local.conf file using CRANK_ENGINE_TARBALL_PATH variable.
python() {
    crank_engine_tarball_path = d.getVar('CRANK_ENGINE_TARBALL_PATH', True)
    if crank_engine_tarball_path:
        premirrors = d.getVar('PREMIRRORS', True)
        d.setVar('PREMIRRORS', "http:///not/exist/crank-sbengine-.* %s \\n %s" % (crank_engine_tarball_path, premirrors))
}

# Disable tasks not needed for the binary package
do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install () {
	# Install launcher script
	install -d -m 0755 ${D}${bindir}
	install -m 0755 ${WORKDIR}/sb-launcher ${D}${bindir}/sb-launcher

	# Copy the engine
	install -d -m 0755 ${D}${datadir}/crank/sbengine
	cp -drf ${S}/${SBENGINE_NAME}/* ${D}${datadir}/crank/sbengine
	chmod a+rx ${D}${datadir}/crank/sbengine/*
}

FILES:${PN} = " \
    ${bindir}/* \
    ${datadir}/crank/sbengine/* \
"
FILES:${PN}-staticdev += " ${datadir}/crank/sbengine/lib/libgreio.a"

#
# Disable failing QA checks:
#
#   Libraries inside /usr/share (datadir)
#   ELF binaries has relocations in .text
#
INSANE_SKIP:${PN} += "libdir textrel"
INSANE_SKIP:${PN}-dbg += "libdir"

RDEPENDS:${PN} = " \
    alsa-lib \
    glib-2.0 \
    gstreamer1.0 \
    libgstapp-1.0 \
    libxml2 \
    zlib \
"
RDEPENDS:${PN}:append:ccimx6ul = " \
    mtdev \
    tslib \
"
RDEPENDS:${PN}:append:ccimx8m = " \
    libegl-imx \
    libgles2-imx \
    wayland \
"
RDEPENDS:${PN}:append:ccimx8x = " \
    libegl-imx \
    libgles2-imx \
    wayland \
"
RDEPENDS:${PN}:append:ccimx6 = " \
    libegl-imx \
    libgles2-imx \
    mtdev \
    tslib \
"
RDEPENDS:${PN}:append:ccmp15 = " \
    libegl-gcnano \
    libgles2-gcnano \
    wayland \
"

PACKAGE_ARCH = "${MACHINE_ARCH}"
