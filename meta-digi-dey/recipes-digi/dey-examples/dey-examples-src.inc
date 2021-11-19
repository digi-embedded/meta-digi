# Copyright (C) 2019-2021 Digi International Inc.

SRCBRANCH = "dey-3.0/maint"
SRCREV = "ada1185ac67f0e73e0d7f74780484ca95c3f7a5d"

DEY_EXAMPLES_STASH = "${DIGI_MTK_GIT}dey/dey-examples.git;protocol=ssh"
DEY_EXAMPLES_GITHUB = "${DIGI_GITHUB_GIT}/dey-examples.git;protocol=https"

DEY_EXAMPLES_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , '${DEY_EXAMPLES_STASH}', '${DEY_EXAMPLES_GITHUB}', d)}"

SRC_URI = "${DEY_EXAMPLES_GIT_URI};nobranch=1"
