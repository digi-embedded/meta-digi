#
# Copyright (C) 2020-2022, Digi International Inc.
#
SUMMARY = "WebKit packagegroup for DEY image"

PACKAGE_ARCH = "${MACHINE_ARCH}"
inherit packagegroup

# Include WPE WebKit, launcher (cog) and examples
RDEPENDS_${PN} += " \
    cog \
    digi-webkit-examples \
    wpewebkit \
"

RDEPENDS_${PN}_remove_mx8 = " digi-webkit-examples"
RDEPENDS_${PN}_append_mx8 = " connectcore-demo-example"
