# Copyright (C) 2011-2012 Freescale Semiconductor
# Copyright (C) 2012 Digi International
# Released under the MIT license (see COPYING.MIT for the terms)

PR = "${INC_PR}.17"

include linux-imx.inc

COMPATIBLE_MACHINE = "(mxs|mx5)"

SRCREV_mxs = "agonzal/yocto"
LOCALVERSION_mxs = "-mxs+agonzal_yocto"

SRCREV_mx5 = "agonzal/yocto"
LOCALVERSION_mx5 = "-mx5x+agonzal_yocto"