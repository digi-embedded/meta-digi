# Copyright (C) 2017 Digi International Inc.

#
# Remove ICU support for 'ccimx6ul' to save space in the rootfs.
#
# Only 'libicudata' could weight up to 25MB:
#
#   25M  /usr/lib/libicudata.so.57.1
PACKAGECONFIG_remove_ccimx6ul = "icu"

PACKAGE_ARCH = "${MACHINE_ARCH}"
