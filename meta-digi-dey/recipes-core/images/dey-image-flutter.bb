# Copyright (C) 2025, Digi International Inc.

DESCRIPTION = "DEY image with Flutter graphical libraries"
LICENSE = "MIT"

IMAGE_INSTALL = " \
    packagegroup-dey-core \
    ${CORE_IMAGE_EXTRA_INSTALL} \
"

IMAGE_FEATURES += " \
    dey-network \
    dey-flutter \
    eclipse-debug \
    ssh-server-dropbear \
    ${@bb.utils.contains('MACHINE_FEATURES', 'alsa', 'dey-audio', '', d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'bluetooth', 'dey-bluetooth', '', d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'wifi', 'dey-wireless', '', d)} \
"

IMAGE_LINGUAS = ""

inherit core-image
inherit dey-image

IMAGE_ROOTFS_SIZE = "8192"
