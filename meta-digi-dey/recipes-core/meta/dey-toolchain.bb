# Copyright (C) 2024, Digi International Inc.

SUMMARY = "Meta package for building an installable DEY toolchain and SDK"
LICENSE = "MIT"

inherit core-image qt-version
inherit populate_sdk ${QT_POPULATE_SDK}

# Add staticdev packages to SDK
SDKIMAGE_FEATURES:append = " staticdev-pkgs"

SDK_NAME = "${PN}-${MACHINE}"
TOOLCHAIN_OUTPUTNAME = "${SDK_NAME}-${SDK_VERSION}"
