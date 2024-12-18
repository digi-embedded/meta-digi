# Copyright (C) 2024, Digi International Inc.

SUMMARY = "Meta package for building an installable DEY toolchain and SDK"
LICENSE = "MIT"

inherit core-image dey-image-sdk qt-version
inherit populate_sdk ${QT_POPULATE_SDK}

# Add a minimal set of IMAGE_FEATURES to allow for integration with the
# appropriate desktop backend (if any) as well as multimedia support
IMAGE_FEATURES += " \
    ${@bb.utils.contains('DISTRO_FEATURES', 'gstreamer', 'dey-gstreamer', '', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'wayland', 'weston', \
       bb.utils.contains('DISTRO_FEATURES',     'x11', 'x11-base x11-sato', \
                                                       '', d), d)} \
"

# Do not include kernel in the toolchain
PACKAGE_EXCLUDE = " \
    kernel-image-* \
"

SDK_NAME = "${PN}-${MACHINE}"
TOOLCHAIN_OUTPUTNAME = "${SDK_NAME}-${SDK_VERSION}"
