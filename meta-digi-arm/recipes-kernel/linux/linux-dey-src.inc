# Copyright (C) 2018-2022 Digi International

LICENSE = "GPLv2"

LOCALVERSION = "-dey"
SRCREV = "${AUTOREV}"

# Select internal or Github Linux repo
DIGI_LOG_REPO = "linux-2.6.git"
DIGI_MTK_REPO = "linux/linux.git"
GITHUB_REPO = "linux.git"
LINUX_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , \
					oe.utils.conditional('DIGI_GIT', '${DIGI_LOG_GIT}', '${DIGI_GIT}/${DIGI_LOG_REPO}', '${DIGI_GIT}/${DIGI_MTK_REPO};protocol=ssh', d), \
					'${DIGI_GITHUB_GIT}/${GITHUB_REPO};protocol=https', d)}"

SRC_URI = "${LINUX_GIT_URI};branch=${SRCBRANCH}"
SRC_URI_stmp32mpcommon = "${LINUX_GIT_URI};branch=${SRCBRANCH}"

S = "${WORKDIR}/git"
