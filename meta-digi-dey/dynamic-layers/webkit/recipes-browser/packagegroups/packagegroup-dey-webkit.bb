#
# Copyright (C) 2020, Digi International Inc.
#
SUMMARY = "WebKit packagegroup for DEY image"

PACKAGE_ARCH = "${MACHINE_ARCH}"
inherit packagegroup

# Include WPE WebKit and launcher (cog)
RDEPENDS_${PN} += " \
    cog \
    wpewebkit \
"
