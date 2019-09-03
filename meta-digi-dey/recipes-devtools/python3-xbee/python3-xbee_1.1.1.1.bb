# Copyright (C) 2019 Digi International Inc.

require python3-xbee.inc

# This change of directory is needed because the package used in pypi was uploaded
# with a wrong number version. Following versions will not need this patch.
# The tarball name has the 1.1.1.1 version, but once you untar the folder is
# named as 1.1.1 instead of 1.1.1.1. so it fails when using pypi.
S = "${WORKDIR}/${PYPI_PACKAGE}-1.1.1"

LIC_FILES_CHKSUM="file://PKG-INFO;md5=0c518add38e71d88298939007bb83940"

SRC_URI[md5sum] = "a30377d8a55071c2d54d7ae1ca24a50b"
