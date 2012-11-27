# Copyright (C) 2012 Digi International
PR = "${INC_PR}.0"

include recipes-kernel/linux/linux-imx.inc

COMPATIBLE_MACHINE = "(mxs|mx5)"

SRCREV_mxs = "agonzal/yocto"
LOCALVERSION_mxs = "-mxs+agonzal_yocto"


SRCREV_mx5 = "agonzal/yocto"
LOCALVERSION_mx5 = "-mx5x+agonzal_yocto"

SRC_URI = "git://log-sln-git.digi.com/linux-2.6.git \
           file://defconfig \
"

