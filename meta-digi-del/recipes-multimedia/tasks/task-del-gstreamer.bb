#
# Copyright (C) 2012 Digi International.
#
DESCRIPTION = "Gstreamer framework task for DEL image"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/LICENSE;md5=3f40d7994397109285ec7b81fdeb3b58"
PACKAGE_ARCH = "${MACHINE_ARCH}"
ALLOW_EMPTY = "1"
PR = "r0"

PACKAGES = "\
	task-del-gstreamer \
	task-del-gstreamer-dbg \
	task-del-gstreamer-dev \
    "

RDEPENDS_task-del-gstreamer = "\
    fsl-mm-codeclib \
    imx-lib \
    ${MACHINE_ESSENTIAL_EXTRA_RDEPENDS}"

RRECOMMENDS_task-del-gstreamer = "\
    ${MACHINE_ESSENTIAL_EXTRA_RRECOMMENDS}"
