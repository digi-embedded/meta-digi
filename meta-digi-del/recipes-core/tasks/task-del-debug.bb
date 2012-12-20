#
# Copyright (C) 2012 Digi International.
#
DESCRIPTION = "Debug applications task for DEL image"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://${COREBASE}/LICENSE;md5=3f40d7994397109285ec7b81fdeb3b58"
PACKAGE_ARCH = "${MACHINE_ARCH}"
ALLOW_EMPTY = "1"
PR = "r0"

#
# Set by the machine configuration with packages essential for device bootup
#
MACHINE_ESSENTIAL_EXTRA_RDEPENDS ?= ""
MACHINE_ESSENTIAL_EXTRA_RRECOMMENDS ?= ""

PACKAGES = "\
	task-del-debug \
	task-del-debug-dbg \
	task-del-debug-dev \
    "

RDEPENDS_task-del-debug = "\
    task-core-tools-debug \
    memwatch \
    fbtest \
    ${MACHINE_ESSENTIAL_EXTRA_RDEPENDS}"

RRECOMMENDS_task-del-debug = "\
    ${MACHINE_ESSENTIAL_EXTRA_RRECOMMENDS}"


