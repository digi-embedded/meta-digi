# Copyright (C) 2012 Digi International.

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"

LIC_FILES_CHKSUM = "file://COPYING;md5=64e753fa7d1ca31632bc383da3b57c27"
SRC_URI = "git://sources.progress-linux.org/git/users/daniel/packages/${PN}.git;protocol=git;branch=debian"

SRCREV = "debian/4.2.0-1"
