# Copyright (C) 2013 Digi International.

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"

# Clean "already stripped" warnings
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
