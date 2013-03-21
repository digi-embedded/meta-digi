# Copyright (C) 2012 Digi International

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"

SRCREV_mxs = "${AUTOREV}"
KBRANCH_mxs = "refs/heads/master"

SRCREV_mx5 = "${AUTOREV}"
KBRANCH_mx5 = "refs/heads/master"

SRC_URI = "${DIGI_LOG_GIT}linux-2.6.git;protocol=git;branch=${KBRANCH} "
