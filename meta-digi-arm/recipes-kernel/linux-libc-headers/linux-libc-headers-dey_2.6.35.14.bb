# Copyright (C) 2012 Digi International

require recipes-kernel/linux-libc-headers/linux-libc-headers.inc

PROVIDES = "linux-libc-headers"
RPROVIDES_${PN}-dev = "linux-libc-headers-dev"
RPROVIDES_${PN}-dbg = "linux-libc-headers-dbg"

require recipes-kernel/linux/linux-dey-rev_${PV}.inc

S = "${WORKDIR}/git"

PR = "r0"
