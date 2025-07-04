#
# Copyright (C) 2022-2025, Digi International Inc.
#

# Inherit custom DIGI sign class to skip signing tool and key parsing restrictions
inherit sign-stm32mp-digi

# Select internal or Github OPTEE repo
OPTEE_URI_STASH = "${DIGI_MTK_GIT}/emp/optee_os.git;protocol=ssh"
OPTEE_URI_GITHUB = "${DIGI_GITHUB_GIT}/optee_os.git;protocol=https"
OPTEE_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , '${OPTEE_URI_STASH}', '${OPTEE_URI_GITHUB}', d)}"

SRCBRANCH = "4.0.0/stm/maint"
SRCREV = "${AUTOREV}"

SRC_URI = " \
    ${OPTEE_GIT_URI};branch=${SRCBRANCH};name=os \
    file://fonts.tar.gz;subdir=git;name=fonts \
"
