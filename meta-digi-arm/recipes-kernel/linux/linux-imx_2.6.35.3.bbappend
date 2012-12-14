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
