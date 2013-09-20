# Copyright (C) 2012 Digi International

SRCREV_external = ""
SRCREV_internal = "35ac8de23d2f0b189855d0a95d20349405e01642"
SRCREV = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRCREV_internal}', '${SRCREV_external}', d)}"

SRC_URI_external = "${DIGI_GITHUB_GIT}/yocto-linux.git;protocol=git"
SRC_URI_internal = "${DIGI_LOG_GIT}linux-2.6.git;protocol=git"
SRC_URI = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRC_URI_internal}', '${SRC_URI_external}', d)}"
