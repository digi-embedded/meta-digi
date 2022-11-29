# Copyright (C) 2019-2022 Digi International Inc.

SRCBRANCH = "dey-4.0/maint"
SRCREV = "d9e0146e41f5d7002fac251e5a69be5a2bb2d7ce"

DEY_EXAMPLES_STASH = "${DIGI_MTK_GIT}/dey/dey-examples.git;protocol=ssh"
DEY_EXAMPLES_GITHUB = "${DIGI_GITHUB_GIT}/dey-examples.git;protocol=https"

DEY_EXAMPLES_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , '${DEY_EXAMPLES_STASH}', '${DEY_EXAMPLES_GITHUB}', d)}"

SRC_URI = "${DEY_EXAMPLES_GIT_URI};nobranch=1"
