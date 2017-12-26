# Copyright (C) 2017, Digi International Inc.

DESCRIPTION = "DEY image including Amazon Web Services packages"
LICENSE = "MIT"

AWS_PACKAGES ?= " \
    awsiotsdk-demo \
    greengrass \
"

IMAGE_INSTALL = " \
    packagegroup-dey-core \
    ${AWS_PACKAGES} \
    ${CORE_IMAGE_EXTRA_INSTALL} \
"

IMAGE_FEATURES += " \
    dey-network \
    package-management \
    ssh-server-dropbear \
    ${@bb.utils.contains('MACHINE_FEATURES', 'bluetooth', 'dey-bluetooth', '', d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'wifi', 'dey-wireless', '', d)} \
"

IMAGE_LINGUAS = ""

inherit core-image
inherit dey-image

IMAGE_ROOTFS_SIZE = "8192"

# Do not install udev-cache
BAD_RECOMMENDATIONS += "udev-cache"
