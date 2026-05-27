# Copyright (C) 2020-2026, Digi International Inc.

SUMMARY = "Short videos to demonstrate video playback on the WPE WebKit"
DESCRIPTION = "This package contains fragments of the short film 'Big Buck Bunny', which are used to demonstrate how WebKit makes use of hardware acceleration for video decoding"
HOMEPAGE = "https://peach.blender.org/"
LICENSE = "CC-BY-3.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/CC-BY-3.0;md5=dfa02b5755629022e267f10b9c0a2ab7"

SRC_URI = "${DIGI_PKG_SRC}/${BPN}-${PV}.tar.gz"

SRC_URI[md5sum] = "5b4cf8fe878adc6105df88866038e6db"
SRC_URI[sha256sum] = "18d64ec814d1a318641b1afc2ff51f93326390fc07dc2b79e53fb92477a0e8bd"

S = "${WORKDIR}/${PN}-${PV}"

WEBSERVER_ROOT = "srv/www"

# List of video sample formats
VIDEO_FORMATS = " \
    mov \
    webm \
"
VIDEO_FORMATS:ccimx95 = " \
    mp4 \
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

PACKAGE_ARCH = "${MACHINE_ARCH}"
