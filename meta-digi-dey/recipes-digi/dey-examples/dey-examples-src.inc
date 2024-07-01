# Copyright (C) 2019-2022 Digi International Inc.

SRCBRANCH = "dey-4.0/maint"
SRCREV = "78402f0e3e138023f70d9577faa11212a9dc3f61"

DEY_EXAMPLES_STASH = "${DIGI_MTK_GIT}/dey/dey-examples.git;protocol=ssh"
DEY_EXAMPLES_GITHUB = "${DIGI_GITHUB_GIT}/dey-examples.git;protocol=https"

DEY_EXAMPLES_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , '${DEY_EXAMPLES_STASH}', '${DEY_EXAMPLES_GITHUB}', d)}"

SRC_URI = "${DEY_EXAMPLES_GIT_URI};nobranch=1"
