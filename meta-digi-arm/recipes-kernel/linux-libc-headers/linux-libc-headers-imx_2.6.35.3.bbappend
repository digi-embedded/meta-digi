# Copyright (C) 2012 Digi International

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"

require recipes-kernel/linux/linux-dey-rev.inc

