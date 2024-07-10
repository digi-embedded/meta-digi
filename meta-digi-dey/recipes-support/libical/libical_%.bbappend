# Copyright (C) 2017-2023, Digi International Inc.

#
# Remove ICU support for platforms with little storage memory to save space
# in the rootfs.
#
# Only 'libicudata' could weight up to 25MB:
#
#   25M  /usr/lib/libicudata.so.57.1
PACKAGECONFIG:remove:ccimx6ul = "icu"
PACKAGECONFIG:remove:ccmp1 = "icu"

PACKAGE_ARCH = "${MACHINE_ARCH}"
