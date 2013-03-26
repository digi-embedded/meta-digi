# Copyright (C) 2013 Digi International.

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}/${MACHINE}/:"

SRC_URI_append_ccimx51js = " file://imx-audio" 
SRC_URI_append_ccimx53js = " file://imx-audio" 

