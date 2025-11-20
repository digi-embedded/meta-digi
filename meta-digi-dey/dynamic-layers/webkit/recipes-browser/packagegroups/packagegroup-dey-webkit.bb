#
# Copyright (C) 2020-2025, Digi International Inc.
#
SUMMARY = "WebKit packagegroup for DEY image"

PACKAGE_ARCH = "${MACHINE_ARCH}"
inherit packagegroup

CC_DEMO_PACKAGE ?= "connectcore-demo-example-multimedia connectcore-demo-example-webkit"
CC_DEMO_PACKAGE:ccmp1 ?= "connectcore-demo-example-webkit"

# Include WPE WebKit, launcher (cog) and examples
RDEPENDS:${PN} += " \
    cog \
    ${CC_DEMO_PACKAGE} \
    wpewebkit \
"
