#
# Copyright (C) 2012-2025, Digi International Inc.
#
SUMMARY = "Debug applications packagegroup for DEY image"

PACKAGE_ARCH = "${MACHINE_ARCH}"
inherit packagegroup

RDEPENDS:${PN} = "\
    evtest \
    i2c-tools \
    memwatch \
    packagegroup-core-tools-debug \
    tcpdump \
"
