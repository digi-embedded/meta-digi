# Copyright (C) 2013 by Digi International Inc.

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"
FILESEXTRAPATHS_prepend := "${THISDIR}/${MACHINE}/:"
