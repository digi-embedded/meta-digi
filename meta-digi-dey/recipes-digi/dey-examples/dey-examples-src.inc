# Copyright (C) 2019, Digi International Inc.

SRCBRANCH = "dey-2.6/maint"
SRCREV = "67a10c2d19ad9740d43cddac9c244e3f7d2c43df"

DEY_EXAMPLES_STASH = "${DIGI_MTK_GIT}dey/dey-examples.git;protocol=ssh"
DEY_EXAMPLES_GITHUB = "${DIGI_GITHUB_GIT}/dey-examples.git;protocol=git"

DEY_EXAMPLES_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , '${DEY_EXAMPLES_STASH}', '${DEY_EXAMPLES_GITHUB}', d)}"
