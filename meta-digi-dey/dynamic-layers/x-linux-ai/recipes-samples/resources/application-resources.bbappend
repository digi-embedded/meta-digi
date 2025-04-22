# Copyright (C) 2024,2025, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += " \
    file://patches/0001-config_board-fix-support-for-web-camera-with-STM32MP.patch \
    file://patches/0002-setup_camera_main_isp-fix-support-for-web-camera.patch \
    file://patches/0003-setup_camera_main_isp-fix-support-for-CSI-DCMIPP-cam.patch \
"
