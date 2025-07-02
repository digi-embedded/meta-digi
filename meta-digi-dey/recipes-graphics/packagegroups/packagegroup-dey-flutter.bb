#
# Copyright (C) 2025, Digi International Inc.
#
SUMMARY = "Flutter packagegroup for DEY image"

PACKAGE_ARCH = "${MACHINE_ARCH}"
inherit packagegroup

RDEPENDS:${PN} += " \
    flutter-pi \
    flutter-samples-veggieseasons \
    flutter-samples-veggieseasons-init \
"

RDEPENDS:${PN}:append:imxgpu = " \
    imx-gpu-viv \
"
