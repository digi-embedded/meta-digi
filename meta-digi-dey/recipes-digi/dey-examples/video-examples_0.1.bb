# Copyright (C) 2020-2022 Digi International.

SUMMARY = "Short videos to demonstrate video playback on the WPE WebKit"
DESCRIPTION = "This package contains fragments of the short film 'Big Buck Bunny', which are used to demonstrate how WebKit makes use of hardware acceleration for video decoding"
HOMEPAGE = "https://peach.blender.org/"
LICENSE = "CC-BY-3.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/CC-BY-3.0;md5=dfa02b5755629022e267f10b9c0a2ab7"

SRC_URI = "${DIGI_PKG_SRC}/${BPN}-${PV}.tar.gz"

SRC_URI[md5sum] = "d22cc0fa20fde187455b27a799d2f9e6"
SRC_URI[sha256sum] = "97389f33d98c52d4311117366f0aa8dc78d00f51a787697af349de4668ccdbf6"

S = "${WORKDIR}/${PN}-${PV}"

WEBSERVER_ROOT = "srv/www"

# List of video sample formats
VIDEO_FORMATS = " \
    mov \
    webm \
"
# Name of the video sample
VIDEO_NAME = "big_buck_bunny"

# The package contains video files, no need to configure or compile
do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
	install -d ${D}/${WEBSERVER_ROOT}/videos

	for format in ${VIDEO_FORMATS}; do
		install -m 644 ${S}/${VIDEO_NAME}.${format} ${D}/${WEBSERVER_ROOT}/videos
	done
}

# All packages involved in the webkit examples install their files in the
# webserver directory
FILES:${PN} = "/${WEBSERVER_ROOT}/*"

# Don't generate dbg or dev packages
PACKAGES = "${PN}"
