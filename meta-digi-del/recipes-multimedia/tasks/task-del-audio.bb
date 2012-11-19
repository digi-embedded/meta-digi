#
# Copyright (C) 2012 Digi International.
#
DESCRIPTION = "Audio task for DEL image"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/LICENSE;md5=3f40d7994397109285ec7b81fdeb3b58"
PACKAGE_ARCH = "${MACHINE_ARCH}"
ALLOW_EMPTY = "1"
PR = "r0"

PACKAGES = "\
	task-del-audio \
	task-del-audio-dbg \
	task-del-audio-dev \
    "

RDEPENDS_task-del-audio = "\
    alsa-lib \
    alsa-utils \
    ${MACHINE_ESSENTIAL_EXTRA_RDEPENDS}"

RRECOMMENDS_task-del-audio = "\
    ${MACHINE_ESSENTIAL_EXTRA_RRECOMMENDS}"


