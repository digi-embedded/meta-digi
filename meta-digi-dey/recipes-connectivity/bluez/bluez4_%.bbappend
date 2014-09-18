# Copyright (C) 2013 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

EXTRA_OECONF_append = " --enable-health"
