# Copyright (C) 2017 Digi International Inc.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

LIBSOC_URI_STASH = "${DIGI_MTK_GIT}dey/libsoc.git;protocol=ssh"
LIBSOC_URI_GITHUB = "git://github.com/jackmitch/libsoc.git;protocol=git"
LIBSOC_URI ?= "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${LIBSOC_URI_STASH}', '${LIBSOC_URI_GITHUB}', d)}"

SRCREV = "dc62bb1f04c13d0423078b1af2bb439c62023d6c"
SRC_URI = " \
    ${LIBSOC_URI};nobranch=1 \
    file://0001-gpio-pwm-add-delay-to-allow-udev-rules-to-complete.patch \
"

PACKAGECONFIG = "python"
