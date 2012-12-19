# Copyright (C) 2012 Digi International
PR_append = "+del.r0"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}_${PV}"

SRCREV_mxs = "agonzal/yocto"
LOCALVERSION_mxs = "mxs+agonzal_yocto"

SRCREV_mx5 = "agonzal/yocto"
LOCALVERSION_mx5 = "mx5+agonzal_yocto"

SRC_URI = "${DIGI_LOG_GIT}linux-2.6.git \
           file://defconfig \
"

# Override 'ccardxmx28js' SRC_URI so the 'SRC_URI_append_mxs' patches
# from FSL layer are *not* applied
SRC_URI_ccardxmx28js_mxs = "${DIGI_LOG_GIT}linux-2.6.git \
           file://defconfig \
"
