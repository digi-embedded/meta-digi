# Copyright (C) 2025, Digi International Inc.

SUMMARY = "Chromium packagegroup for DEY"
DESCRIPTION = "Packages required to run the Digi Getting Started demo application on Chromium Wayland browser"

inherit packagegroup

RDEPENDS:${PN} += "\
    chromium-ozone-wayland \
    connectcore-demo-example-chromium \
"
