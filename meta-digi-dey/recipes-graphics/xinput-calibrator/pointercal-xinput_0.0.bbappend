# Copyright (C) 2013 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${MACHINE}:"

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"
