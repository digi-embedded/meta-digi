# Copyright (C) 2017 Digi International Inc.

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-CMakeList-add-option-to-build-without-ICU-support.patch"

# Remove inconditional 'icu' dependence if not set through PACKAGECONFIG
DEPENDS_remove = "${@bb.utils.contains('PACKAGECONFIG', 'icu', '', 'icu', d)}"

PACKAGECONFIG ?= "icu"
PACKAGECONFIG[icu] = "-DWITH_LIBICU=1,-DWITH_LIBICU=0"

#
# Remove ICU support for 'ccimx6ul' to save space in the rootfs.
#
# Only 'libicudata' could weight up to 25MB:
#
#   25M  /usr/lib/libicudata.so.57.1
PACKAGECONFIG_remove_ccimx6ul = "icu"

PACKAGE_ARCH = "${MACHINE_ARCH}"
