#
# Copyright (C) 2022, Digi International Inc.
#
SUMMARY = "Crank packagegroup for DEY image"

PACKAGE_ARCH = "${MACHINE_ARCH}"
inherit packagegroup

# Include Crank engine and demos
RDEPENDS:${PN} += " \
    crank-demos \
    crank-sbengine \
"
