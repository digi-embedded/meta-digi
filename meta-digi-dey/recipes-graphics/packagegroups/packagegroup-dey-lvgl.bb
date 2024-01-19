#
# Copyright (C) 2023 Digi International Inc.
#
SUMMARY = "LVGL packagegroup for DEY image"

PACKAGE_ARCH = "${MACHINE_ARCH}"
inherit packagegroup

RDEPENDS:${PN} += " \
    lvgl-demo \
"
