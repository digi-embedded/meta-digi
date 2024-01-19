# Copyright 2024 Digi International Inc.

#
# Reuse meta-freescale's opencv_4.6.0.imx.bb
#
require recipes-support/opencv/opencv_4.6.0.imx.bb

SRC_URI:remove = "file://0001-Add-missing-header-for-LIBAVCODEC_VERSION_INT.patch"

SRCBRANCH = "4.7.0_imx"
SRCREV_opencv = "3acf6a50fcb4f774728d2338553ad646ccc14b14"

# Update opencv_contrib
SRC_URI:remove = "git://github.com/opencv/opencv_contrib.git;destsuffix=git/contrib;name=contrib;branch=master;protocol=https"
SRC_URI += "git://github.com/opencv/opencv_contrib.git;destsuffix=git/contrib;name=contrib;branch=4.x;protocol=https"
SRCREV_contrib = "e247b680a6bd396f110274b6c214406a93171350"

SRC_URI:remove = "git://github.com/opencv/opencv_extra.git;destsuffix=extra;name=extra;branch=master;protocol=https"
SRC_URI =+ "git://github.com/opencv/opencv_extra.git;destsuffix=extra;name=extra;branch=4.x;protocol=https"

SRCREV_extra = "5abbd7e0546bbb34ae7487170383d3e571fb1dd1"

COMPATIBLE_MACHINE = "(mx9-nxp-bsp)"
