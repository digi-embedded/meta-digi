# Copyright (C) 2025,2026 Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:append:dey = " \
    file://0001-boards-ccimx95-add-platform-as-a-clone-of-mx95lp5.patch \
    file://0002-ddr-add-DDR-configuration-file-for-ccimx95.patch \
    file://0003-ccimx95-configure-console-on-LPUART6.patch \
    file://0004-ccimx95-add-DDR-configuration-file-for-ccimx95-B0-si.patch \
    file://0005-ccimx95-configure-console-on-LPUART1.patch \
"
# NXP's 'lf-6.6.52_2.2.2' release
SRCREV = "49bfaa93e9d1fe213866bcb9507927a59a9ede5a"

OEI_DEBUG:dey = "1"
