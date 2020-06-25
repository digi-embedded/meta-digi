# Copyright (C) 2019, Digi International Inc.

SRCBRANCH = "dey-3.0/maint"
SRCREV = "1317384494f56ac3d10e7dee3ac968b944f02468"

DEY_EXAMPLES_STASH = "${DIGI_MTK_GIT}dey/dey-examples.git;protocol=ssh"
DEY_EXAMPLES_GITHUB = "${DIGI_GITHUB_GIT}/dey-examples.git;protocol=https"

DEY_EXAMPLES_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , '${DEY_EXAMPLES_STASH}', '${DEY_EXAMPLES_GITHUB}', d)}"

SRC_URI = "${DEY_EXAMPLES_GIT_URI};nobranch=1"
