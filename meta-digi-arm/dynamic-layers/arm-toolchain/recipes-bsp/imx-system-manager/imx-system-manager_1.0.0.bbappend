# Copyright (C) 2025, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:append:dey = " \
    file://0001-ccimx95dvk-add-new-platform-config-and-board.patch \
    file://0002-ccimx95dvk-configure-board-and-switch-debug-UART-to-.patch \
    file://0003-ccimx95dvk-disable-PCAL6408A-expander-and-move-GPIO1.patch \
    file://0004-ccimx95dvk-move-resources-from-M7-to-A55.patch \
    file://0005-ccimx95dvk-move-pads-to-non-secure-A55.patch \
    file://0006-ccimx95dvk-move-CAN1-to-be-used-by-A55.patch \
    file://0007-ccimx95dvk-remove-PCAL6408A-IO-expander-from-EVK.patch \
    file://0008-ccimx95dvk-remove-PCA2123-RTC-from-EVK.patch \
    file://0009-ccimx95-change-names-of-voltage-regulators.patch \
    file://0010-ccimx95dvk-enable-full-access-to-certain-regulators-.patch \
    file://0011-components-pf09-reduce-LDOs-step-to-50mV.patch \
    file://0012-ccimx95dvk-remove-access-to-VDD_3V3-and-VDD_1V8-from.patch \
"

# Disable debug monitor by default
PACKAGECONFIG ??= "m0"
