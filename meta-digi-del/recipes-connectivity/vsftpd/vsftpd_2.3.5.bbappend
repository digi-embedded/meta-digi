# Copyright (C) 2013 Digi International.

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"

FILESEXTRAPATHS_prepend := "${THISDIR}/files"

DEPENDS += "openssl"

# Inhibit warning about files already stripped
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
