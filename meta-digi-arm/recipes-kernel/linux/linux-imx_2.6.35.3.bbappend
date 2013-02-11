# Copyright (C) 2012 Digi International
PR_append = "+del.r0"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}_${PV}"

SRCREV_mxs = "del-6.0.1-sprint4"
LOCALVERSION_mxs = "mxs+del-6.0.1-sprint4"

SRCREV_mx5 = "del-6.0.1-sprint4"
LOCALVERSION_mx5 = "mx5+del-6.0.1-sprint4"

SRC_URI = "${DIGI_LOG_GIT}linux-2.6.git \
           file://defconfig \
"

# Override 'ccardxmx28js' SRC_URI so the 'SRC_URI_append_mxs' patches
# from FSL layer are *not* applied
SRC_URI_ccardxmx28js_mxs = "${DIGI_LOG_GIT}linux-2.6.git \
           file://defconfig \
"
FILES_kernel-image += "/boot/config*"
