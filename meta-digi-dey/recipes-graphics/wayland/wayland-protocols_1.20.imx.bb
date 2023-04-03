SUMMARY = "Collection of additional Wayland protocols"
DESCRIPTION = "Wayland protocols that add functionality not \
available in the Wayland core protocol. Such protocols either add \
completely new functionality, or extend the functionality of some other \
protocol either in Wayland core, or some other protocol in \
wayland-protocols."
HOMEPAGE = "http://wayland.freedesktop.org"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://COPYING;md5=c7b12b6702da38ca028ace54aae3d484 \
                    file://stable/presentation-time/presentation-time.xml;endline=26;md5=4646cd7d9edc9fa55db941f2d3a7dc53"

WAYLAND_PROTOCOLS_SRC ?= "git://github.com/nxp-imx/wayland-protocols-imx.git;protocol=https"
SRCBRANCH = "wayland-protocols-imx-1.20"
SRC_URI = "${WAYLAND_PROTOCOLS_SRC};branch=${SRCBRANCH} "
SRCREV = "9cacf108d0ee5863c7a656da5d2271bc2396e43d" 
S = "${WORKDIR}/git"

UPSTREAM_CHECK_URI = "https://wayland.freedesktop.org/releases.html"

inherit autotools pkgconfig

PACKAGES = "${PN}"
FILES_${PN} += "${datadir}/pkgconfig/wayland-protocols.pc"

PACKAGE_ARCH = "${MACHINE_SOCARCH}"
COMPATIBLE_MACHINE = "(imxfbdev|imxgpu)"
