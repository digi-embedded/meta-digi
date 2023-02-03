#
# Copyright (C) 2020-2023, Digi International Inc.
#
SUMMARY = "WebKit packagegroup for DEY image"

PACKAGE_ARCH = "${MACHINE_ARCH}"
inherit packagegroup

# Include WPE WebKit, launcher (cog) and examples
RDEPENDS:${PN} += " \
    cog \
    connectcore-demo-example-webkit-multimedia \
    wpewebkit \
"
