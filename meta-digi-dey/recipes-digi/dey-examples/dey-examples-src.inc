# Copyright (C) 2019, Digi International Inc.

SRCBRANCH = "dey-2.6/maint"
SRCREV = "e8baba6de87ff637afe68493957ec114b2aa38d7"

DEY_EXAMPLES_STASH = "${DIGI_MTK_GIT}dey/dey-examples.git;protocol=ssh"
DEY_EXAMPLES_GITHUB = "${DIGI_GITHUB_GIT}/dey-examples.git;protocol=git"

DEY_EXAMPLES_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , '${DEY_EXAMPLES_STASH}', '${DEY_EXAMPLES_GITHUB}', d)}"
