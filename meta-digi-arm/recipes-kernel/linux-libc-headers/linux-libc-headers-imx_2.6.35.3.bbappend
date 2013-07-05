# Copyright (C) 2012 Digi International

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"

SRCREV_external = "aeca956315a7b2e6e4cc94f11fc799fcfa791353"
SRCREV_internal = "feec20411f1ee6c507bf50d12268cd8c05fe6820"
SRCREV_forcevariable = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRCREV_internal}', '${SRCREV_external}', d)}"

SRC_URI_external = "${DIGI_GITHUB_GIT}/yocto-linux.git;protocol=git"
SRC_URI_internal = "${DIGI_LOG_GIT}linux-2.6.git;protocol=git"
SRC_URI = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRC_URI_internal}', '${SRC_URI_external}', d)}"

