# Copyright (C) 2019-2022 Digi International Inc.

SRCBRANCH = "dey-4.0/maint"
SRCREV = "26f613f2e432a988f4b5c96c8de43652d7ad2d5d"

DEY_EXAMPLES_STASH = "${DIGI_MTK_GIT}/dey/dey-examples.git;protocol=ssh"
DEY_EXAMPLES_GITHUB = "${DIGI_GITHUB_GIT}/dey-examples.git;protocol=https"

DEY_EXAMPLES_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , '${DEY_EXAMPLES_STASH}', '${DEY_EXAMPLES_GITHUB}', d)}"

SRC_URI = "${DEY_EXAMPLES_GIT_URI};nobranch=1"
