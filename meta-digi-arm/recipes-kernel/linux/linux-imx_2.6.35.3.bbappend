# Copyright (C) 2012 Digi International
PR_append = "+del.r0"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}_${PV}"

SRCREV_mxs = "master"
LOCALVERSION_mxs = "mxs+master"

SRCREV_mx5 = "master"
LOCALVERSION_mx5 = "mx5+master"

SRC_URI = "${DIGI_LOG_GIT}linux-2.6.git \
           file://defconfig \
"

# Override 'ccardimx28js' SRC_URI so the 'SRC_URI_append_mxs' patches
# from FSL layer are *not* applied
SRC_URI_ccardimx28js_mxs = "${DIGI_LOG_GIT}linux-2.6.git \
           file://defconfig \
"
FILES_kernel-image += "/boot/config*"
