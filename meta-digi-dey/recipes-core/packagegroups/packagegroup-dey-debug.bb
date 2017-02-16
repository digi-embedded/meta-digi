#
# Copyright (C) 2012 Digi International.
#
SUMMARY = "Debug applications packagegroup for DEY image"

PACKAGE_ARCH = "${MACHINE_ARCH}"
inherit packagegroup

RDEPENDS_${PN} = "\
    evtest \
    fbtest \
    i2c-tools \
    memwatch \
    packagegroup-core-tools-debug \
    tcpdump \
"
