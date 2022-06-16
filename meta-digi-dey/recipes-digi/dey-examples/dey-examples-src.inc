# Copyright (C) 2019-2021 Digi International Inc.

SRCBRANCH = "dey-3.2/maint"
SRCREV = "8176b68510b3f5b67f14d6c8d82aae786f40ddeb"

DEY_EXAMPLES_STASH = "${DIGI_MTK_GIT}/dey/dey-examples.git;protocol=ssh"
DEY_EXAMPLES_GITHUB = "${DIGI_GITHUB_GIT}/dey-examples.git;protocol=https"

DEY_EXAMPLES_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , '${DEY_EXAMPLES_STASH}', '${DEY_EXAMPLES_GITHUB}', d)}"

SRC_URI = "${DEY_EXAMPLES_GIT_URI};nobranch=1"
