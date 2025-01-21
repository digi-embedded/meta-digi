# Copyright (C) 2024,2025, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += " \
    file://patches/0001-config_board-add-support-to-STM32MP255-processor.patch \
    file://patches/0002-config_board-fix-support-for-web-camera-with-STM32MP.patch \
    file://patches/0003-setup_camera_main_isp-fix-support-for-web-camera.patch \
"
