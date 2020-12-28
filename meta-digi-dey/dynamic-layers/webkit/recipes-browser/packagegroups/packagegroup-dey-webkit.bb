#
# Copyright (C) 2020, Digi International Inc.
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
