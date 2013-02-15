# Copyright (C) 2012 Digi International
PR_append = "+del.r0"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}_${PV}"

SRCREV_mx5 = "${AUTOREV}"
LOCALVERSION_mx5 = "mx5+master"

SRC_URI = "${DIGI_LOG_GIT}linux-2.6.git;protocol=git;branch=refs/heads/master \
           file://defconfig \
"

# Override 'ccardimx28js' SRC_URI so the 'SRC_URI_append_mxs' patches
# from FSL layer are *not* applied
SRCREV_mxs = "${AUTOREV}"
LOCALVERSION_mxs = "mxs+master"
SRC_URI_ccardimx28js_mxs = "${DIGI_LOG_GIT}linux-2.6.git;protocol=git;branch=refs/heads/master \
           file://defconfig \
"
FILES_kernel-image += "/boot/config*"
