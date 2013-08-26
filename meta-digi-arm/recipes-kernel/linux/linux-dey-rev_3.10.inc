# Copyright (C) 2012 Digi International

SRCREV_external = ""
SRCREV_internal = "3b9f8c6ad5d91c5d6ae87567cf7386faf28ee3e4"
SRCREV = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRCREV_internal}', '${SRCREV_external}', d)}"

SRC_URI_external = "${DIGI_GITHUB_GIT}/yocto-linux.git;protocol=git"
SRC_URI_internal = "${DIGI_LOG_GIT}linux-2.6.git;protocol=git"
SRC_URI = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRC_URI_internal}', '${SRC_URI_external}', d)}"
