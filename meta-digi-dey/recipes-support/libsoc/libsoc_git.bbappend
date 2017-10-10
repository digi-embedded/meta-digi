# Copyright (C) 2017 Digi International Inc.

LIBSOC_URI_STASH = "${DIGI_MTK_GIT}dey/libsoc.git;protocol=ssh"
LIBSOC_URI_GITHUB = "git://github.com/jackmitch/libsoc.git;protocol=git"
LIBSOC_URI ?= "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${LIBSOC_URI_STASH}', '${LIBSOC_URI_GITHUB}', d)}"

SRC_URI = " \
    ${LIBSOC_URI};branch=${SRCBRANCH} \
"

PACKAGECONFIG = "python"

