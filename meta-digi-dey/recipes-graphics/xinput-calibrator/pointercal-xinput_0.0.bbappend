# Copyright (C) 2013 Digi International.

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}/${MACHINE}/${PREFERRED_VERSION_linux-dey}:${THISDIR}/${PN}/${MACHINE}:"
