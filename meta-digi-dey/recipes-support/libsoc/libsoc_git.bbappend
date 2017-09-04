# Copyright (C) 2017 Digi International Inc.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BP}:"

LIBSOC_URI_STASH = "${DIGI_MTK_GIT}dey/libsoc.git;protocol=ssh"
LIBSOC_URI_GITHUB = "git://github.com/jackmitch/libsoc.git;protocol=git"
LIBSOC_URI ?= "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${LIBSOC_URI_STASH}', '${LIBSOC_URI_GITHUB}', d)}"

SRC_URI = " \
    ${LIBSOC_URI};branch=${SRCBRANCH} \
    file://board.conf \
"

PACKAGECONFIG = "enableboardconfig python"

do_configure_prepend() {
	install -m 0644 ${WORKDIR}/board.conf ${S}/contrib/board_files/${BOARD}.conf
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
