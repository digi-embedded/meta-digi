# Copyright (C) 2012 Digi International
PR_append = "+digi.0"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}_${PV}"
SRCREV_ccxmx51js = "agonzal/yocto"
SRCREV_ccxmx53js = "agonzal/yocto"
SRCREV_ccardxmx28js = "agonzal/yocto"
SRC_URI = "${DIGI_LOG_GIT}linux-2.6.git \
           file://defconfig \
"

LOCALVERSION_ccxmx51js = "mx5+agonzal_yocto"
LOCALVERSION_ccxmx53js = "mx5+agonzal_yocto"
LOCALVERSION_ccardxmx28js = "mxs+agonzal_yocto"
