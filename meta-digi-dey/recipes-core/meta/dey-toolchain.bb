# Copyright (C) 2024, Digi International Inc.

SUMMARY = "Meta package for building an installable DEY toolchain and SDK"
LICENSE = "MIT"

inherit core-image dey-image-sdk qt-version
inherit populate_sdk ${QT_POPULATE_SDK}

# Do not include kernel in the toolchain
PACKAGE_EXCLUDE = " \
    kernel-image-* \
"

SDK_NAME = "${PN}-${MACHINE}"
TOOLCHAIN_OUTPUTNAME = "${SDK_NAME}-${SDK_VERSION}"
