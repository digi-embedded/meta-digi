# Copyright (C) 2012 Digi International
PR_append = "+digi.0"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}_${PV}"
SRC_URI = "git://log-sln-git.digi.com/linux-2.6.git;tag=agonzal/yocto \
           file://defconfig \
"

LOCALVERSION_mx5 = "mx5+agonzal_yocto"
LOCALVERSION_mxs = "mxs+agonzal_yocto"
