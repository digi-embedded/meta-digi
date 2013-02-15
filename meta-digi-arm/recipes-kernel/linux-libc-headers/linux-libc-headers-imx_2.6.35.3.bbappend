# Copyright (C) 2012 Digi International
PR_append = "+del.r0"

SRCREV_mxs = "${AUTOREV}"
KBRANCH_mxs = "refs/heads/master"

SRCREV_mx5 = "${AUTOREV}"
KBRANCH_mx5 = "refs/heads/master"

SRC_URI = "${DIGI_LOG_GIT}linux-2.6.git;protocol=git;branch=${KBRANCH} "
